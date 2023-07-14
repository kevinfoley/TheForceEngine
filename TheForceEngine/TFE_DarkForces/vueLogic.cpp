#include <cstring>

#include "vueLogic.h"
#include "time.h"
#include <TFE_Game/igame.h>
#include <TFE_DarkForces/Actor/actor.h>
#include <TFE_Jedi/Memory/allocator.h>
#include <TFE_Jedi/InfSystem/message.h>
#include <TFE_Jedi/Level/level.h>
#include <TFE_Jedi/Level/levelData.h>
#include <TFE_Jedi/Level/rwall.h>
#include <TFE_Jedi/Collision/collision.h>
#include <TFE_System/system.h>
#include <TFE_FileSystem/paths.h>
#include <TFE_FileSystem/filestream.h>
#include <TFE_Jedi/Serialization/serialization.h>

using namespace TFE_Jedi;

namespace TFE_DarkForces
{
	enum VueFlags
	{
		VUE_PAUSED = FLAG_BIT(0),
	};

	enum VueFrameFlags
	{
		VFRAME_FIRST = FLAG_BIT(0),
	};

	struct VueLogic
	{
		Logic logic;

		Allocator* frames;
		Task* task;
		s32  isCamera;
		Tick frameDelay;
		RSector* sector;
		u32 flags;
	};

	struct VueFrame
	{
		fixed16_16 mtx[9];
		vec3_fixed offset;

		angle14_16 pitch;
		angle14_16 yaw;
		angle14_16 roll;
		u32 flags;
	};

	// Make sure this is local to the current file.
	namespace
	{
		struct LocalContext
		{
			VueLogic* vue;
			JBool searchForSector;
			SecObject* obj;
			VueFrame* frame;
			Tick tick;
			Tick pauseTick;
			s32 prevFrame;
		};
	}

	static char* s_workBuffer = nullptr;
	static size_t s_workBufferSize = 0;

	JBool vueLogicSetupFunc(Logic* logic, KEYWORD key);
	void vueLogicTaskFunc(MessageType msg);
	void vueLogicCleanupFunc(Logic *logic);

	Logic* obj_createVueLogic(SecObject* obj, LogicSetupFunc* setupFunc)
	{
		VueLogic* vueLogic = (VueLogic*)level_alloc(sizeof(VueLogic));

		vueLogic->logic.obj = obj;
		vueLogic->frames = nullptr;
		vueLogic->frameDelay = 9;	// 9 Ticks between frames = ~16 fps
		vueLogic->flags = 0;

		obj->entityFlags |= ETFLAG_CAN_WAKE;

		Task* task = createSubTask("vueLogic", vueLogicTaskFunc);
		vueLogic->task = task;
		task_setUserData(task, vueLogic);

		obj_addLogic(obj, (Logic*)vueLogic, LOGIC_VUE, task, vueLogicCleanupFunc);
		*setupFunc = vueLogicSetupFunc;

		return (Logic*)vueLogic;
	}
		
	void vueLogic_serializeTaskLocalMemory(Stream* stream, void* userData, void* mem)
	{
		VueLogic* self = (VueLogic*)userData;
		LocalContext* locals = (LocalContext*)mem;
		bool modeWrite = serialization_getMode() == SMODE_WRITE;
		if (!modeWrite)
		{
			locals->vue = self;
		}
		SERIALIZE(SaveVersionInit, locals->searchForSector, 0);
		s32 objIndex = -1;
		if (modeWrite)
		{
			objIndex = locals->obj->serializeIndex;
		}
		SERIALIZE(SaveVersionInit, objIndex, -1);
		if (!modeWrite)
		{
			locals->obj = nullptr;
			if (objIndex >= 0)
			{
				locals->obj = objData_getObjectBySerializationId(objIndex);
			}
		}
		s32 index = -1;
		if (modeWrite && self->frames && locals->frame)
		{
			index = allocator_getIndex(self->frames, locals->frame);
		}
		SERIALIZE(SaveVersionInit, index, -1);
		if (!modeWrite)
		{
			locals->frame = (VueFrame*)allocator_getByIndex(self->frames, index);
		}
		SERIALIZE(SaveVersionInit, locals->tick, 0);
		SERIALIZE(SaveVersionInit, locals->pauseTick, 0);
		SERIALIZE(SaveVersionInit, locals->prevFrame, 0);
	}

	// Serialization
	void vueLogic_serialize(Logic*& logic, SecObject* obj, Stream* stream)
	{
		VueLogic* vueLogic;
		if (serialization_getMode() == SMODE_WRITE)
		{
			vueLogic = (VueLogic*)logic;
		}
		else
		{
			vueLogic = (VueLogic*)level_alloc(sizeof(VueLogic));
			logic = (Logic*)vueLogic;

			Task* task = createSubTask("vueLogic", vueLogicTaskFunc);
			vueLogic->task = task;
			task_setUserData(task, vueLogic);

			vueLogic->logic.task = task;
			vueLogic->logic.cleanupFunc = vueLogicCleanupFunc;
		}

		s32 frameCount;
		if (serialization_getMode() == SMODE_WRITE)
		{
			// Frames
			frameCount = allocator_getCount(vueLogic->frames);
			SERIALIZE(ObjState_InitVersion, frameCount, 0);
			allocator_saveIter(vueLogic->frames);
				VueFrame* frame = (VueFrame*)allocator_getHead(vueLogic->frames);
				while (frame)
				{
					SERIALIZE(ObjState_InitVersion, (*frame), { 0 });
					frame = (VueFrame*)allocator_getNext(vueLogic->frames);
				}
			allocator_restoreIter(vueLogic->frames);
		}
		else
		{
			SERIALIZE(ObjState_InitVersion, frameCount, 0);
			vueLogic->frames = allocator_create(sizeof(VueFrame));
			for (s32 i = 0; i < frameCount; i++)
			{
				VueFrame* frame = (VueFrame*)allocator_newItem(vueLogic->frames);
				SERIALIZE(ObjState_InitVersion, (*frame), { 0 });
			}
		}

		SERIALIZE(ObjState_InitVersion, vueLogic->isCamera, 0);
		SERIALIZE(ObjState_InitVersion, vueLogic->frameDelay, 0);
		serialization_serializeSectorPtr(stream, ObjState_InitVersion, vueLogic->sector);
		SERIALIZE(ObjState_InitVersion, vueLogic->flags, 0);

		task_serializeState(stream, vueLogic->logic.task, vueLogic, vueLogic_serializeTaskLocalMemory);
	}

	void loadVueFile(Allocator* vueList, char* transformName, TFE_Parser* parser)
	{
		size_t bufferPos = 0;
		if (!strcasecmp(transformName, "camera"))
		{
			while (1)
			{
				const char* line = parser->readLine(bufferPos);
				if (!line) { break; }

				f32 x1, z1, y1, x2, z2, y2, r, lens;
				if (sscanf(line, "camera %f %f %f %f %f %f %f %f", &x1, &z1, &y1, &x2, &z2, &y2, &r, &lens) == 8)
				{
					y1 = -y1;
					y2 = -y2;
					VueFrame* frame = (VueFrame*)allocator_newItem(vueList);
					frame->offset.x = floatToFixed16(x1);
					frame->offset.y = floatToFixed16(y1);
					frame->offset.z = floatToFixed16(z1);
					frame->yaw   = vec2ToAngle(floatToFixed16(x2 - x1), floatToFixed16(z2 - z1));
					frame->pitch = 0;
					frame->roll  = angle14_32(r * 16383.0f / 360.0f);
				}
			}
		}
		else
		{
			// Matrix 0
			fixed16_16 mtx0[9];
			mtx0[0] = ONE_16;
			mtx0[1] = 0;
			mtx0[2] = 0;

			mtx0[3] = 0;
			mtx0[4] = 1;
			mtx0[5] = ONE_16 - 1;

			mtx0[6] = 0;
			mtx0[7] = -ONE_16 + 1;
			mtx0[8] = 1;

			// Matrix 1
			fixed16_16 mtx1[9];
			mtx1[0] = ONE_16;
			mtx1[1] = 0;
			mtx1[2] = 0;

			mtx1[3] = 0;
			mtx1[4] = 1;
			mtx1[5] = ONE_16 - 1;

			mtx1[6] = 0;
			mtx1[7] = -ONE_16 + 1;
			mtx1[8] = 1;

			VueFrame* frame = (VueFrame*)allocator_newItem(vueList);
			frame->flags = VFRAME_FIRST;

			while (1)
			{
				const char* line = parser->readLine(bufferPos);
				if (!line)
				{
					break;
				}

				char name[32];
				f32 f00, f01, f02, f03, f04, f05, f06, f07, f08, f09, f10, f11;
				s32 count = sscanf(line, "transform %s %f %f %f %f %f %f %f %f %f %f %f %f", name, &f00, &f01, &f02, &f03, &f04, &f05, &f06, &f07, &f08, &f09, &f10, &f11);
				if (count == 13)
				{
					// Is this the correct transform?
					if (transformName[0] == '*' || strcasecmp(name, transformName) == 0)
					{
						frame = (VueFrame*)allocator_newItem(vueList);

						// Rotation/Scale matrix.
						fixed16_16 frameMtx[9];
						frameMtx[0] = floatToFixed16(f00);
						frameMtx[1] = floatToFixed16(f01);
						frameMtx[2] = floatToFixed16(f02);
						frameMtx[3] = floatToFixed16(f03);
						frameMtx[4] = floatToFixed16(f04);
						frameMtx[5] = floatToFixed16(f05);
						frameMtx[6] = floatToFixed16(f06);
						frameMtx[7] = floatToFixed16(f07);
						frameMtx[8] = floatToFixed16(f08);

						// Transform to DF coordinate system.
						fixed16_16 tempMtx[9];
						mulMatrix3x3(mtx1, frameMtx, tempMtx);
						mulMatrix3x3(tempMtx, mtx0, frame->mtx);

						frame->offset.x =  floatToFixed16(f09);
						frame->offset.y = -floatToFixed16(f11);
						frame->offset.z =  floatToFixed16(f10);

						frame->yaw = 8191;
						frame->flags = 0;
					}
				}
			}
		}
	}

	void vue_resetState()
	{
		s_workBufferSize = 0;
		s_workBuffer = nullptr;
	}
		
	char* allocateWorkBuffer(size_t size)
	{
		if (size > s_workBufferSize)
		{
			s_workBufferSize = size + 1024;
			s_workBuffer = (char*)game_realloc(s_workBuffer, s_workBufferSize);
		}
		assert(s_workBuffer);
		return s_workBuffer;
	}

	JBool createParserFromFile(FilePath* filePath, TFE_Parser* parser)
	{
		assert(filePath && parser);

		FileStream file;
		size_t size = 0;
		char* buffer = nullptr;
		if (file.open(filePath, Stream::MODE_READ))
		{
			size = file.getSize();
			buffer = allocateWorkBuffer(size);
			file.readBuffer(buffer, (u32)size);
		}
		file.close();

		if (!buffer || !size)
		{
			return JFALSE;
		}

		parser->init(buffer, size);
		parser->addCommentString("//");
		parser->addCommentString("#");
		return JTRUE;
	}

	Allocator* key_loadVue(char* arg1, char* arg2, s32 isCamera)
	{
		FilePath filePath;
		if (!TFE_Paths::getFilePath(arg1, &filePath))
		{
			TFE_System::logWrite(LOG_ERROR, "VUE", "key_loadVue: COULD NOT OPEN.");
			return nullptr;
		}
		TFE_Parser parser;
		if (!createParserFromFile(&filePath, &parser))
		{
			TFE_System::logWrite(LOG_ERROR, "VUE", "key_loadVue: COULD NOT OPEN.");
			return nullptr;
		}
		
		Allocator* vueList = allocator_create(sizeof(VueFrame));
		loadVueFile(vueList, arg2, &parser);
		
		return vueList;
	}

	Allocator* key_appendVue(char* arg1, char* arg2, Allocator* frames)
	{
		FilePath filePath;
		if (!TFE_Paths::getFilePath(arg1, &filePath))
		{
			TFE_System::logWrite(LOG_ERROR, "VUE", "key_appendVue: COULD NOT OPEN.");
			return frames;
		}
		TFE_Parser parser;
		if (!createParserFromFile(&filePath, &parser))
		{
			TFE_System::logWrite(LOG_ERROR, "VUE", "key_appendVue: COULD NOT OPEN.");
			return frames;
		}

		loadVueFile(frames, arg2, &parser);
		return frames;
	}

	void key_setViewFrames(VueLogic* vueLogic, Allocator* frames, s32 isCamera)
	{
		vueLogic->frames = frames;
		vueLogic->isCamera = isCamera;
		task_makeActive(vueLogic->task);
	}

	JBool vueLogicSetupFunc(Logic* logic, KEYWORD key)
	{
		VueLogic* vueLogic = (VueLogic*)logic;
		char* endPtr = nullptr;

		if (key == KW_VUE)
		{
			s32 isCamera = 0;
			if (s_objSeqArgCount < 3)
			{
				s_objSeqArg2[0] = '*';
			}
			else
			{
				__strupr(s_objSeqArg2);
				if (strstr(s_objSeqArg2, "CAMERA"))
				{
					isCamera = 1;
				}
			}
			Allocator* frames = key_loadVue(s_objSeqArg1, s_objSeqArg2, isCamera);
			key_setViewFrames(vueLogic, frames, isCamera);
			return JTRUE;
		}
		else if (key == KW_VUE_APPEND)
		{
			s32 isCamera = 0;
			if (s_objSeqArgCount < 3)
			{
				s_objSeqArg2[0] = '*';
			}
			else
			{
				__strupr(s_objSeqArg2);
				if (strstr(s_objSeqArg2, "CAMERA"))
				{
					isCamera = 1;
				}
			}
			key_appendVue(s_objSeqArg1, s_objSeqArg2, vueLogic->frames);
			return JTRUE;
		}
		else if (key == KW_FRAME_RATE)
		{
			vueLogic->frameDelay = time_frameRateToDelay(strtof(s_objSeqArg1, &endPtr));
			return JTRUE;
		}
		else if (key == KW_PAUSE)
		{
			vueLogic->flags |= VUE_PAUSED;
			return JTRUE;
		}
		return JFALSE;
	}

	void vueLogicTaskFunc(MessageType msg)
	{
		task_begin_ctx;

		local(vue) = (VueLogic*)task_getUserData();
		local(searchForSector) = JTRUE;
		local(obj) = local(vue)->logic.obj;
		local(vue)->sector = local(obj)->sector;
		
		while (msg != MSG_FREE_TASK)
		{
			if (local(vue)->frames)
			{
				if (local(vue)->isCamera > 0)
				{
					// The original code sets the camera pointer to the VUE.
					// But then this is never used.
					//
					// s_camera = local(vue);
				}

				local(frame) = (VueFrame*)allocator_getHead(local(vue)->frames);
				local(searchForSector) = JTRUE;
				local(tick) = s_curTick;
				local(prevFrame) = 0;
				if (!local(frame))
				{
					break;
				}

				while (local(frame))
				{
					if (local(frame)->flags & VFRAME_FIRST)
					{
						if (local(vue)->flags & VUE_PAUSED)
						{
							local(pauseTick) = s_curTick;
							entity_yield(TASK_SLEEP);

							task_makeActive(task_getCurrent());
							local(tick) += s_curTick - local(pauseTick);
							entity_yield(TASK_NO_DELAY);
						}
						local(frame) = (VueFrame*)allocator_getNext(local(vue)->frames);
					}
					else
					{
						task_localBlockBegin;
							memcpy(local(obj)->transform, local(frame)->mtx, 9 * sizeof(fixed16_16));
							RSector* newSector = nullptr;

							JBool useCollision = JFALSE;
							if (local(vue)->sector && local(vue)->sector != s_levelState.controlSector)
							{
								useCollision = !local(searchForSector);
							}

							if (useCollision)
							{
								RWall* wall = collision_wallCollisionFromPath(local(vue)->sector, local(obj)->posWS.x, local(obj)->posWS.z, local(frame)->offset.x, local(frame)->offset.z);
								while (wall)
								{
									if (wall->nextSector)
									{
										newSector = wall->nextSector;
										wall = collision_pathWallCollision(newSector);
									}
									else
									{
										break;
									}
								}
								if (wall)
								{
									newSector = s_levelState.controlSector;
								}
							}
							else
							{
								newSector = sector_which3D(local(frame)->offset.x, local(frame)->offset.y, local(frame)->offset.z);
								if (!newSector)
								{
									newSector = s_levelState.controlSector;
								}
								local(searchForSector) = JFALSE;
							}

							if (newSector)
							{
								sector_addObject(newSector, local(obj));
								local(vue)->sector = newSector;
							}

							local(obj)->posWS = local(frame)->offset;
							local(obj)->yaw = local(frame)->yaw;
						task_localBlockEnd;

						entity_yield(TASK_NO_DELAY);
						if (msg == MSG_FREE_TASK) { break; }

						Tick dt = s_curTick - local(tick);
						s32 frameIndex = dt / local(vue)->frameDelay;
						for (; local(prevFrame) != frameIndex && local(frame); local(prevFrame)++)
						{
							local(frame) = (VueFrame*)allocator_getNext(local(vue)->frames);
							if (!local(frame) || ((local(vue)->flags & VUE_PAUSED) && (local(frame)->flags & VFRAME_FIRST)))
							{
								break;
							}
						}
					}
				}  // while (frame)
			}
			else
			{
				entity_yield(TASK_SLEEP);
			}
		}  // while (msg != MSG_FREE_TASK)

		deleteLogicAndObject((Logic*)local(vue));
		task_end;
	}

	void vueLogicCleanupFunc(Logic *logic)
	{
		deleteLogicAndObject(logic);
		task_free(logic->task);
	}

}  // TFE_DarkForces