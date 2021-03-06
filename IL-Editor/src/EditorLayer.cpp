#include "EditorLayer.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scripts/Scripts.h"

#include "IL/Scene/SceneSerializer.h" 
#include "IL/Utils/PlatformUtils.h"

namespace IL
{

	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		IL_PROFILE_FUNCTION();

		m_Texture1 = Texture2D::Create("assets/textures/CheckerBoard.png");
		m_Texture2 = Texture2D::Create("assets/textures/ILLmew.png");
		m_TileMap = Texture2D::Create("assets/textures/mapPack_tilesheet_2X.png");

		m_TreeTex = SubTexture2D::CreateFromCoord(m_TileMap, { 3, 4 }, { 128, 128 });
		m_SnowmanTex = SubTexture2D::CreateFromCoord(m_TileMap, { 3, 5 }, { 128, 128 });
		m_FloorTex = SubTexture2D::CreateFromCoord(m_TileMap, { 0, 5 }, { 128, 128 }, { 3, 3 });

		FrameBufferProps myProps;
		myProps.Width = 1280;
		myProps.Height = 720;
		m_FrameBuffer = FrameBuffer::Create(myProps);

		m_ActiveScene = CreateRef<Scene>();

#if 0
		m_SquareEntityL = m_ActiveScene->CreateEntity("SquareL");
		m_SquareEntityR = m_ActiveScene->CreateEntity("SquareR");
		m_SquareEntityT = m_ActiveScene->CreateEntity("SquareT");
		m_SquareEntityB = m_ActiveScene->CreateEntity("SquareB");
		m_SquareEntityFront = m_ActiveScene->CreateEntity("SquareFront");
		m_SquareEntityBack = m_ActiveScene->CreateEntity("SquareBack");
		m_ScriptEntity = m_ActiveScene->CreateEntity("Scripted Entity");

		m_SquareEntityL.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });
		m_SquareEntityR.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });
		m_SquareEntityT.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });
		m_SquareEntityB.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });
		m_SquareEntityFront.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });
		m_SquareEntityBack.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.8f, 0.9f, 0.2f, 1.0f });

		m_ScriptEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		m_FirstCameraEntity = m_ActiveScene->CreateEntity("First Camera");
		m_SecondCameraEntity = m_ActiveScene->CreateEntity("Second Camera");

		m_FirstCameraEntity.AddComponent<CameraComponent>();
		m_SecondCameraEntity.AddComponent<CameraComponent>();
		m_SecondCameraEntity.GetComponent<CameraComponent>().Primary = false;

		glm::vec3 mainOffset = { -1.5f, 0.5f, 0.0f };

		m_SquareEntityL.GetComponent<TransformComponent>().Translation.x = -0.5f;
		m_SquareEntityL.GetComponent<TransformComponent>().Translation += mainOffset;
		m_SquareEntityL.GetComponent<TransformComponent>().Rotation.y = glm::radians(90.0f);

		m_SquareEntityR.GetComponent<TransformComponent>().Translation.x = 0.5f;
		m_SquareEntityR.GetComponent<TransformComponent>().Translation += mainOffset;
		m_SquareEntityR.GetComponent<TransformComponent>().Rotation.y = glm::radians(90.0f);
																					
		m_SquareEntityT.GetComponent<TransformComponent>().Translation.y = 0.5f;	
		m_SquareEntityT.GetComponent<TransformComponent>().Translation += mainOffset;
		m_SquareEntityT.GetComponent<TransformComponent>().Rotation.x = glm::radians(90.0f);
																					
		m_SquareEntityB.GetComponent<TransformComponent>().Translation.y = -0.5f;	
		m_SquareEntityB.GetComponent<TransformComponent>().Translation += mainOffset;
		m_SquareEntityB.GetComponent<TransformComponent>().Rotation.x = glm::radians(90.0f);

		m_SquareEntityFront.GetComponent<TransformComponent>().Translation.z = -0.5f;
		m_SquareEntityFront.GetComponent<TransformComponent>().Translation += mainOffset;
		m_SquareEntityBack.GetComponent<TransformComponent>().Translation.z = 0.5f;
		m_SquareEntityBack.GetComponent<TransformComponent>().Translation += mainOffset;

		m_ScriptEntity.AddComponent<NativeScriptComponent>().Bind<SquareAnimated>();

		m_FirstCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

		m_SceneHierachyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDeatch()
	{
		IL_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(TimeStep dt)
	{
		IL_PROFILE_FUNCTION();

		Renderer2D::ResetStats();
		{
			IL_PROFILE_SCOPE("EditorLayer::Prep");
			m_FrameBuffer->Bind();
			RenderCommand::SetClearColor({ 0.1, 0.1, 0.1, 1.0 });
			RenderCommand::Clear();
		}

		{
			IL_PROFILE_SCOPE("EditorLayer::RenderDrawCall");
			m_ActiveScene->OnUpdate(dt);
			m_FrameBuffer->UnBind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
#pragma region Imgui::DockSpace
		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			// Always be the top of the view
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float windowMinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 400.0f;

		// hold shift to dock
		io.ConfigDockingWithShift = true;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = windowMinSizeX;
#pragma endregion Imgui::DockSpace

		if (ImGui::BeginMenuBar())
		{
			static bool saveScenePopup = false;
			if (ImGui::BeginMenu("File and settings"))
			{
				ImGui::Separator();
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();
				if (ImGui::MenuItem("exit"))
					Application::GetApplication().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();

		// Panels
		m_SceneHierachyPanel.OnImGuiRender();

#pragma region Statistics
#if IL_DEBUG
		auto stats = Renderer2D::GetStats();

		ImGui::Begin("Statistics");  // Statistics
		ImGui::Text("DrawCalls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		uint32_t texture = m_Texture2->GetRendererID();
		ImGui::Image((void*)texture, ImVec2{ 256.0f, 256.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();                // Statistics
#endif
#pragma endregion Statistics

#pragma region ImGui::Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport");  // Viewport
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		// Draw frameBuffer on viewport
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		uint32_t colorAttachTexture = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)colorAttachTexture, viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		// Resize viewport
		if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y)
		{
			m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_ViewportSize = { viewportSize.x, viewportSize.y };

			m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			m_ViewportSize.x = viewportSize.x;
			m_ViewportSize.y = viewportSize.y;
		}
		ImGui::End();  // Viewport

		ImGui::PopStyleVar();
#pragma endregion ImGui::Viewport
	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(IL_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(IL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(IL_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(IL_KEY_LEFT_SHIFT) || Input::IsKeyPressed(IL_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case IL_KEY_N:
			{
				if (control)
					NewScene();
				break;
			}
			case IL_KEY_O:
			{
				if (control)
					OpenScene();
				break;
			}
			case IL_KEY_S:
			{
				if (control && shift)
					SaveSceneAs();
				break;
			}
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierachyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("IL Scene (*.iscene)\0*.iscene\0");
		if (!filepath.empty())
		{
			NewScene();
			SceneSerializer serializer(m_ActiveScene);
			serializer.DeserializeYaml(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("IL Scene (*.iscene)\0*.iscene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.SerializeYaml(filepath);
		}
	}

}