#include <engine/ui/ui.h>

namespace engine::ui {

	UI::UI(GLFWwindow * window, double deltaTime, MenuState state, std::string startQuest) {
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		menuState = state;
		questLog = startQuest;
	}

	UI::~UI() {
		ImGui::DestroyContext();
	}

	void UI::drawDebugUI(double deltaTime) {
		ImGui::GetCurrentContext();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		{
			ImGuiIO &io = ImGui::GetIO();
			io.DeltaTime = deltaTime;
		}
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();
		ImGui::Text("Frames per Seconds: %06.3f fps", 1 / deltaTime);
		ImGui::EndMainMenuBar();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UI::drawGameUI(double deltaTime, double time) {
		ImGui::GetCurrentContext();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		{
			ImGuiIO &io = ImGui::GetIO();
			io.DeltaTime = deltaTime;
		}
		ImGui::NewFrame();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		ImGui::BeginMainMenuBar();

		switch (menuState)
		{

		case ResourceMenu:
			ImGui::Image((void*)(intptr_t)woodImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", wood);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)stoneImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", stone);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)oreImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", ore);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)cornImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", corn);

			ImGui::Dummy(ImVec2(1400.0f, 0.0f));
			ImGui::Text("Main Menu");
			ImGui::Image((void*)(intptr_t)escImage.id(), ImVec2(32, 32));
			break;

		case BuildMenu:

			ImGui::Image((void*)(intptr_t)woodImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", wood);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)stoneImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", stone);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)oreImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", ore);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)cornImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", corn);

			ImGui::Dummy(ImVec2(25.0f, 0.0f));

			ImGui::Text("Resources needed for building = ");
			ImGui::Image((void*)(intptr_t)woodImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", neededWood);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)stoneImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", neededStone);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)oreImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", neededOre);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Image((void*)(intptr_t)cornImage.id(), ImVec2(32, 32));
			ImGui::Text("%0d", neededCorn);

			ImGui::Dummy(ImVec2(825.0f, 0.0f));

			ImGui::Text("Build");
			ImGui::Image((void*)(intptr_t)mouseLeftImage.id(), ImVec2(32, 32));

			break;

		case MainMenu:
			ImGui::Text("Back");
			ImGui::Image((void*)(intptr_t)backImage.id(), ImVec2(32, 32));
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Restart Game");
			ImGui::Image((void*)(intptr_t)spaceImage.id(), ImVec2(32, 32));
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Toggle Quest Log");
			ImGui::Image((void*)(intptr_t)altImage.id(), ImVec2(32, 32));
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Exit Game");
			ImGui::Image((void*)(intptr_t)escImage.id(), ImVec2(32, 32));
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Controls");
			ImGui::Image((void*)(intptr_t)wasdImage.id(), ImVec2(50, 32));
			ImGui::Image((void*)(intptr_t)mouseImage.id(), ImVec2(32, 32));
			break;

		case ScoreMenu:
			actualTime = time - startTime;

			ImGui::Text("Main Menu");
			ImGui::Image((void*)(intptr_t)escImage.id(), ImVec2(32, 32));
			
			ImGui::Dummy(ImVec2(750.0f, 0.0f));

			ImGui::Text("Blocks %0d", blocks);
			ImGui::Text("/ %0d", 51);

			ImGui::Dummy(ImVec2(500.0f, 0.0f));

			ImGui::Text("Actual Time %0.3f", actualTime);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Best Time %0.3f", bestTime);
			break;

		case ResultMenu:
			ImGui::Text("Main Menu");
			ImGui::Image((void*)(intptr_t)escImage.id(), ImVec2(32, 32));
			ImGui::Dummy(ImVec2(750.0f, 0.0f));
			ImGui::Text("Your Time %0.3f", actualTime);
			ImGui::Dummy(ImVec2(25.0f, 0.0f));
			ImGui::Text("Best Time %0.3f", bestTime);
			break;

		default:
			break;
		}

		ImGui::EndMainMenuBar();
		ImGui::PopStyleVar(1);
		
		if (isQuestLogActive) {
			//ImGui::SetWindowFontScale(1.25);
			ImGui::GetForegroundDrawList()->AddText(ImVec2(5, 50), 0xffffffff, "Quest Log:");
			ImGui::GetForegroundDrawList()->AddText(ImVec2(15, 75), 0xffffffff, questLog.c_str());
		}
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UI::loadIcons(std::filesystem::path paths[11]) {
		woodImage.loadImage(paths[0]);
		stoneImage.loadImage(paths[1]);
		oreImage.loadImage(paths[2]);
		cornImage.loadImage(paths[3]);

		escImage.loadImage(paths[4]);
		spaceImage.loadImage(paths[5]);
		backImage.loadImage(paths[6]);
		altImage.loadImage(paths[7]);
		mouseLeftImage.loadImage(paths[8]);
		mouseImage.loadImage(paths[9]);
		wasdImage.loadImage(paths[10]);
	}

}