/////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////

int clicks = 0;
void main()
{
	imgui.SetNextWindowPos(100, 60);
	imgui.SetNextWindowSize(500, 0);
	imgui.Begin("Test", 0);
	imgui.PushFont(FONT_MENU);
	imgui.Text("Hello, world");
	imgui.PopFont();
	imgui.Separator();
	imgui.TextWrapped("This UI is being drawn from ForceScript!");
	
	if (imgui.Button("Click me"))
	{
		clicks++;
	}

	imgui.Text("" + clicks + " clicks");
	imgui.End();
}
