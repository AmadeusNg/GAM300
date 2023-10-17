/*!*************************************************************************
****
\file ImguiProperties.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the Level Editor Properties
		panel
****************************************************************************
***/


#include "imguiHelper/ImGuiFunctionHelper.h"

#include "imguiHelper/ImguiProperties.h"
#include "imguiHelper/ImguiHierarchy.h"

namespace TDS
{
	/*!*************************************************************************
	This function initializes the Properties panel
	****************************************************************************/
	Properties::Properties()
	{
		flags = ImGuiWindowFlags_NoCollapse;
		panelTitle = "Properties";
		windowPadding = ImVec2(0.f, 0.f);
	}

	/*!*************************************************************************
	This function is the update function for Properties panel
	****************************************************************************/
	void Properties::update()
	{
		std::shared_ptr<Hierarchy> hierarchyPanel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[PanelTypes::HIERARCHY]);

		// If there is a selected entity
		if (EntityID selectedEntity = hierarchyPanel->getSelectedEntity())
		{
			std::vector<std::string> allComponentNames = ecs.getAllRegisteredComponents();

			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_SpanFullWidth |
				ImGuiTreeNodeFlags_Selected;

			int i = 0;
			for (auto componentName : allComponentNames)
			{
				IComponent* componentBase = getComponentByName(componentName, selectedEntity);

				if (componentName == "Name Tag" && ImGui::BeginTable("###", 2))
				{
					ImguiComponentDisplay(componentName, componentBase);
					ImGui::EndTable();

					continue;
				}

				if (!componentBase)
				{
					continue;
				}

				bool componentOpen = ImGui::TreeNodeEx(componentName.c_str(), nodeFlags);

				ImGui::PushID(i);
				if (ImGui::BeginPopupContextItem("componentEditPopup"))
				{
					if (ImGui::Selectable("Remove Component"))
					{
						removeComponentByName(componentName, selectedEntity);
						TDS_INFO("Removed Component");
					}

					ImGui::EndPopup();
				}

				if (rightClick && ImGui::IsItemHovered())
				{
					ImGui::OpenPopup("componentEditPopup");
				}
				ImGui::PopID();
				++i;

				if (componentOpen)
				{
					if (ImGui::BeginTable("###", 2, /*ImGuiTableFlags_Borders |*/ ImGuiTableFlags_NoPadInnerX, ImVec2(0.0f, 5.5f)))
					{
						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						ImGui::TableNextColumn();
						ImGui::PushItemWidth(-FLT_MIN); // Right-aligned

						ImguiComponentDisplay(componentName, componentBase);

						ImGui::PopItemWidth();
						ImGui::EndTable();
					}
					ImGui::TreePop();
				}
			}

			// Add component button
			ImGuiStyle& style = ImGui::GetStyle();

			float size = ImGui::CalcTextSize("Add Component").x + style.FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

			if (ImGui::Button("Add Component", ImVec2(100.f, 20.f)))
			{
				ImGui::OpenPopup("componentAddPopup");
			}

			if (ImGui::BeginPopupContextItem("componentAddPopup"))
			{
				for (auto componentName : allComponentNames)
				{
					if (!getComponentByName(componentName, selectedEntity) && ImGui::Selectable(componentName.c_str()))
					{
						addComponentByName(componentName, selectedEntity);
						TDS_INFO("Added Component");
					}
				}
				ImGui::EndPopup();
			}
		}
	}

	/*!*************************************************************************
	This function is the helper function for Properties panel
	****************************************************************************/
	void Properties::ImguiComponentDisplay(std::string componentName, IComponent* componentBase)
	{
		if (componentName == "Name Tag")
		{
			auto nameTagComponent = dynamic_cast<NameTag*>(componentBase);

			ImGui::Indent();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Name");
			ImGui::Unindent();

			ImGui::TableNextColumn();
			char temp[100];
			strcpy_s(temp, nameTagComponent->GetNameTag().c_str());
			ImGui::InputText("##name", temp, 100);
			nameTagComponent->SetNameTag(std::string(temp));

			//nameTagComponent->SetNameTag(ImguiTextInput("Name", nameTagComponent->GetNameTag()));
		}
		else if (componentName == "Transform")
		{
			auto transformComponent = dynamic_cast<Transform*>(componentBase);
			transformComponent->SetPosition(ImguiVec3Input("Position", transformComponent->GetPosition()));
			transformComponent->SetScale(ImguiVec3Input("Scale", transformComponent->GetScale()));
			transformComponent->SetRotation(ImguiFloatInput("Rotation", transformComponent->GetRotation()));
		}
		else if (componentName == "AI")
		{
			//auto aiComponent = dynamic_cast<AI*>(componentBase);
		}
		else if (componentName == "Camera Component")
		{
			//auto cameraComponent = dynamic_cast<CameraComponent*>(componentBase);
		}
		else if (componentName == "Collider")
		{
			auto colliderComponent = dynamic_cast<Collider*>(componentBase);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Collider Type");

			ImGui::TableNextColumn();

			// Collider Type
			static std::string colliderTypeStringList[3]{ "None", "Circle", "Rectangle" };
			int colliderTypeSelected = static_cast<int>(colliderComponent->GetColliderType()) - 1;

			// Uninitialized
			if (colliderTypeSelected == -1)
			{
				colliderComponent->SetColliderType(Collider::ColliderType::NONE);
				colliderTypeSelected = 0;
			}

			if (ImGui::BeginCombo("##", colliderTypeStringList[colliderTypeSelected].c_str()))
			{
				for (int n = 0; n < 3; n++)
				{
					const bool is_selected = (colliderTypeSelected == n);
					if (ImGui::Selectable(colliderTypeStringList[n].c_str(), is_selected))
					{
						colliderComponent->SetColliderType(static_cast<Collider::ColliderType>(n + 1));
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			colliderComponent->SetColliderNormal(ImguiVec3Input("Collider Normal", colliderComponent->GetColliderNormal()));
			colliderComponent->SetMin(ImguiVec3Input("Min", colliderComponent->GetMin()));
			colliderComponent->SetMax(ImguiVec3Input("Max", colliderComponent->GetMax()));
			colliderComponent->SetOffset(ImguiVec3Input("Offset", colliderComponent->GetOffset()));
			colliderComponent->SetHit(ImguiIntInput("Hit", colliderComponent->GetHit()));
			colliderComponent->SetRadius(ImguiFloatInput("Radius", colliderComponent->GetRadius()));
			colliderComponent->SetAlive(ImguiBoolInput("Alive", colliderComponent->GetAlive()));

		}
		else if (componentName == "Player Attributes")
		{
			//auto playerAttributesComponent = dynamic_cast<PlayerAttributes*>(componentBase);
		}
		else if (componentName == "Rigid Body")
		{
			auto rigidBodyComponent = dynamic_cast<RigidBody*>(componentBase);
			rigidBodyComponent->SetInputForce(ImguiVec3Input("Input Force", rigidBodyComponent->GetInputForce()));
			rigidBodyComponent->SetLinearVel(ImguiVec3Input("Linear Velocity", rigidBodyComponent->GetLinearVel()));
			rigidBodyComponent->SetAngularVel(ImguiVec3Input("Angular Velocity", rigidBodyComponent->GetAngularVel()));
			rigidBodyComponent->SetAcceleration(ImguiVec3Input("Acceleration", rigidBodyComponent->GetAcceleration()));
			rigidBodyComponent->SetDirection(ImguiVec3Input("Direction", rigidBodyComponent->GetDirection()));
			rigidBodyComponent->SetNextPosition(ImguiVec3Input("Next Position", rigidBodyComponent->GetNextPosition()));
			rigidBodyComponent->SetFrictionCoefficient(ImguiFloatInput("Friction Coefficient", rigidBodyComponent->GetFriction()));
			rigidBodyComponent->SetRestitution(ImguiFloatInput("Restitution", rigidBodyComponent->GetRestitution()));
			rigidBodyComponent->SetMass(ImguiFloatInput("Mass", rigidBodyComponent->GetMass()));
			rigidBodyComponent->SetGravity(ImguiFloatInput("Gravity", rigidBodyComponent->GetGravity()));
		}
		else if (componentName == "Sprite")
		{
			auto spriteComponent = dynamic_cast<Sprite*>(componentBase);
			spriteComponent->SetIndex(ImguiVec2Input("Index", spriteComponent->GetIndex()));
			spriteComponent->SetIsSpriteSheet(ImguiBoolInput("Is Spritesheet", spriteComponent->GetIsSpriteSheet()));
			spriteComponent->SetIsAnimated(ImguiBoolInput("Is Animated", spriteComponent->GetIsAnimated()));
			spriteComponent->SetInternalTimer(ImguiFloatInput("Internal Timer", spriteComponent->GetInternalTimer()));
			spriteComponent->SetLayerOrder(ImguiIntInput("Layer Order", spriteComponent->GetLayerOrder()));
			spriteComponent->SetTextureName(ImguiTextInput("Texture Name", spriteComponent->GetTextureName()));
			spriteComponent->SetUVcooridnates(ImguiVec2Input("UV Coordinate", spriteComponent->GetUVcooridnates()));
			spriteComponent->SetAlpha(ImguiFloatInput("Alpha", spriteComponent->GetAlpha()));
			spriteComponent->SetMaxIndex(ImguiIntInput("Max Index", spriteComponent->GetMaxIndex()));
		}
		else if (componentName == "Tag")
		{
			auto tagComponent = dynamic_cast<Tag*>(componentBase);
			tagComponent->SetTag(ImguiTextInput("Tag", tagComponent->GetTag()));
			tagComponent->SetTargetTag(ImguiTextInput("Target Tag", tagComponent->GetTargetTag()));
		}
		else if (componentName == "Win Data")
		{
			//auto transform = dynamic_cast<WinData*>(componentBase);
		}
	}
}