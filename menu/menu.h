#include "../thirdparty/imgui/imgui.h"
#include "../config/config.h"
#include <MinHook.h>

extern bool b_render_menu;
extern HWND g_hwnd;
extern WNDPROC o_wndproc;

namespace menu
{
	static void setup_style() noexcept
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 13 );
		ImGui::GetStyle().FrameRounding = 4.0f;
		ImGui::GetStyle().GrabRounding = 4.0f;
		ImGui::GetStyle().WindowTitleAlign = { 0.5f, 0.5f };
		const auto style = &ImGui::GetStyle();

		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2( 5, 8 );
		style->FrameRounding = 5.f;
		style->ItemSpacing = ImVec2( 12, 0 );
		style->ItemInnerSpacing = ImVec2( 8, 6 );
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;
		style->WindowBorderSize = 0.f;

		style->Colors[ImGuiCol_Text] = ImVec4( 0.80f, 0.80f, 0.83f, 1.00f );
		style->Colors[ImGuiCol_TextDisabled] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
		style->Colors[ImGuiCol_WindowBg] = ImVec4( 0.06f, 0.05f, 0.07f, 0.95f );
		style->Colors[ImGuiCol_PopupBg] = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
		style->Colors[ImGuiCol_Border] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
		style->Colors[ImGuiCol_BorderShadow] = ImVec4( 0.92f, 0.91f, 0.88f, 0.00f );
		style->Colors[ImGuiCol_FrameBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
		style->Colors[ImGuiCol_TitleBg] = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 1.00f, 0.98f, 0.95f, 0.75f );
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
		style->Colors[ImGuiCol_CheckMark] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
		style->Colors[ImGuiCol_SliderGrab] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
		style->Colors[ImGuiCol_Button] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
		style->Colors[ImGuiCol_ButtonActive] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
		style->Colors[ImGuiCol_Header] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
		style->Colors[ImGuiCol_HeaderActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
		style->Colors[ImGuiCol_PlotLines] = ImVec4( 0.40f, 0.39f, 0.38f, 0.63f );
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.25f, 1.00f, 0.00f, 1.00f );
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.40f, 0.39f, 0.38f, 0.63f );
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 0.25f, 1.00f, 0.00f, 1.00f );
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.25f, 1.00f, 0.00f, 0.43f );
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( 1.00f, 0.98f, 0.95f, 0.73f );
	}

	static void render() noexcept
	{
		static const auto add_item = []( const char* text, bool* option_toggle, bool outgroup = false )
		{
			ImGui::Text( "%s", text );
			ImGui::SameLine( outgroup ? 370.0f : 360.0f );
			ImGui::ToggleButton( text, option_toggle );
		};

		static const auto add_item_with_color = []( const char* id, bool* option_toggle, float* color_array )
		{
			add_item( id, option_toggle );
			ImGui::SameLine( 330 );
			ImGui::ColorEdit3( id, color_array, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs );
		};

		if ( b_render_menu )
		{
			ImGui::SetNextWindowSize( ImVec2( 415.f, 240.f ) );

			if ( ImGui::Begin( "Lazysight" , nullptr , ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |  ImGuiWindowFlags_NoResize ) )
			{
				const char* s_get_nearest_by[2] = { "Aimbot by: Distance", "Aimbot by: Fov" };
				
				static auto tab = 0;
				const auto style = &ImGui::GetStyle();
				style->FramePadding = ImVec2(5, 8);

				ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 2 ) );

				if ( ImGui::Button( "Visuals", { 125 , 0 } ) ) tab = 0;
				
				ImGui::SameLine();

				if ( ImGui::Button( "Aimbot", { 125 , 0 } ) ) tab = 1;
				
				ImGui::SameLine();

				if ( ImGui::Button( "Misc", { 125 , 0 } ) ) tab = 2;
				
				ImGui::PopStyleVar();

				style->FramePadding = ImVec2( 5, 2 );

				ImGui::Text( " " ); // advanced spacing system
				switch ( tab )
				{
					case 0:
						add_item( "Activate", &config::visuals_toggle, true );

						ImGui::Text( " " );

						ImGui::BeginChild( 1, ImVec2( 390, 220 ), true );
						add_item_with_color( "ESP Line Enemy", &config::b_enemy_line, config::enemy_line );
						add_item_with_color( "ESP Line Ally", &config::b_ally_line, config::ally_line );
						add_item_with_color( "ESP Box 2D Enemy", &config::b_enemy_box, config::enemy_box );
						add_item_with_color( "ESP Box 2D Ally", &config::b_ally_box, config::ally_box );
						add_item_with_color( "ESP Name Enemy", &config::b_enemy_name, config::enemy_name );
						add_item_with_color( "ESP Name Ally", &config::b_ally_name, config::ally_name );
						add_item_with_color( "ESP Distance Enemy", &config::b_enemy_distance, config::enemy_distance );
						add_item_with_color( "ESP Distance Ally", &config::b_ally_distance, config::ally_distance );
						add_item( "ESP HP Enemy", &config::b_enemy_hp );
						add_item( "ESP HP Ally", &config::b_ally_hp );
						add_item_with_color( "ESP HP Bar Enemy", &config::b_enemy_hp_bar, config::enemy_hp_bar );
						add_item_with_color( "ESP HP Bar Ally", &config::b_ally_hp_bar, config::ally_hp_bar );
						ImGui::EndChild();

						break;

					case 1:
						add_item( "Aimbot", &config::b_aimbot, true );

						ImGui::Text( " " );

						ImGui::BeginChild( 2, ImVec2( 390, 125 ), true );
						add_item( "Trigger", &config::b_trigger );
						add_item( "Smooth", &config::b_smooth );
						ImGui::SliderInt( "value", &config::i_smooth, 1, 15 );

						add_item( s_get_nearest_by[config::b_nearest_by_fov], &config::b_nearest_by_fov );

						add_item( "Draw fov", &config::b_view_fov );
						ImGui::SliderInt( "value ", &config::i_fov, 1, 360 );
						ImGui::SameLine();
						ImGui::ColorEdit4( "fov_color", config::view_fov, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel );
						ImGui::EndChild();
						break;

					case 2:
						ImGui::Text( "Aim Correction" );
						ImGui::BeginChild( 3, ImVec2( 390, 50 ), true );
						style->ItemSpacing = ImVec2( 12, 3 );
						add_item( "No Recoil", &config::b_recoil );
						add_item( "No Spread", &config::b_spread );
						style->ItemSpacing = ImVec2( 12, 0 );
						ImGui::EndChild();

						ImGui::Text( "Player" );
						ImGui::BeginChild( 4, ImVec2( 390, 65 ), true );
						add_item( "Stamina", &config::b_stamina );
						add_item( "Speed Hack", &config::b_speed );
						ImGui::SliderInt( "*", &config::i_speed, 1, 30 );
						ImGui::EndChild();

						ImGui::Text( " " );
						ImGui::PushStyleColor( ImGuiCol_Button, { 1.f , 0.f , 0.f , 1.f } );

						if ( ImGui::Button( "Unload" ) )
						{
							MH_DisableHook( MH_ALL_HOOKS );
							SetWindowLongPtr( g_hwnd, GWLP_WNDPROC, LONG_PTR( o_wndproc ) );
							FreeConsole();
						}

						ImGui::PopStyleColor();
						break;

					default: break;
				}
				ImGui::End();
			}
		}
	}
}