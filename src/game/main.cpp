#include <process.hpp>

const ivec2 screen_size(480, 270);
Interface::Window window("Delta", screen_size * 2, Interface::windowed, Interface::WindowSettings{} with_(min_size = screen_size));
Graphics::DummyVertexArray dummy_vao;

const Graphics::ShaderConfig shader_config = Graphics::ShaderConfig::Core();
Interface::ImGuiController gui_controller(Poly::derived<Interface::ImGuiController::GraphicsBackend_Modern>, Interface::ImGuiController::Config{} with_(shader_header = shader_config.common_header));

Input::Mouse mouse;

ImFont *font_main;
ImFont *font_mono;

constexpr int modal_margin = 64;

namespace States
{
    struct Base : Meta::with_virtual_destructor<Base>
    {
        virtual void Tick() = 0;
        virtual void Render() const = 0;
    };

    Poly::Storage<Base> current_state;

    struct Main : Base
    {
        struct ProcessData
        {
            std::optional<TinyProcessLib::Process> process;
            std::thread thread;
            std::atomic_bool process_ended = 0; // This is set to `true` for one tick after the process ends.
            bool process_running = 0;
            int should_close_popup_automatically = -1; // -1 means not decided yet.
            float popup_alpha = 1;
            bool using_debugger = 0;

            std::string output;
            std::mutex output_mutex;
        };

        std::unique_ptr<ProcessData> process_data = std::make_unique<ProcessData>();

        std::string current_file = "4"; // Without extension.
        std::string current_code;
        std::string current_output;
        bool unsaved_changes = 0;
        bool text_input_focused = 0;

        Main()
        {
            current_code = MemoryFile(current_file + ".gps").string();
            FilterCharacters(current_code);
            LoadOutput();
        }

        void FilterCharacters(std::string &str)
        {
            std::string new_str;
            new_str.reserve(str.size());
            int line_len = 0;
            for (char ch : str)
            {
                if (ch == '\n')
                {
                    line_len = 0;
                    new_str += '\n';
                    continue;
                }

                if (ch == '\t')
                {
                    // We can't do anything fancy here, because ImGui always renders tabs as 4 spaces, and this function shouldn't change how the text looks.
                    constexpr int tab_size = 4;
                    line_len += tab_size;
                    new_str += std::string(tab_size, ' ');
                    continue;
                }

                if (ch >= 0 && ch < ' ')
                    continue;

                line_len++;
                new_str.push_back(ch);
            }

            str = std::move(new_str);
        }

        void SaveChanges()
        {
            if (!unsaved_changes)
                return;

            unsaved_changes = 0;

            try
            {
                std::string str = current_code;
                FilterCharacters(str); // We modify a copy because text editing widget doesn't like its data being modified when it's active.
                const uint8_t *data = reinterpret_cast<const uint8_t *>(str.c_str());
                MemoryFile::Save(current_file + ".gps", data, data + str.size());
            }
            catch (std::exception &e)
            {
                Program::Error("Не могу сохранить файл.\n", e.what());
            }
        }

        void LoadOutput()
        {
            try
            {
                current_output = MemoryFile(current_file + ".lis").string();
                FilterCharacters(current_output);
            }
            catch (...)
            {
                current_output = "";
            }
        }


        void Tick() override
        {
            ImGui::SetNextWindowPos(ivec2(0));
            ImGui::SetNextWindowSize(window.Size());

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ivec2(2,1));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

            ImGui::Begin("main window", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ivec2(4,4));

            bool run = 0;
            bool debug = 0;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Файл"))
                {
                    ImGui::MenuItem("Открыть");

                    if (ImGui::MenuItem("Сохранить", "Ctrl+S"))
                        SaveChanges();

                    ImGui::EndMenu();
                }
                if ((Input::Button(Input::l_ctrl).down() || Input::Button(Input::r_ctrl).down()) && Input::Button(Input::s).pressed())
                    SaveChanges();

                if (ImGui::MenuItem("Запустить (F9)") || Input::Button(Input::f9).pressed())
                    run = 1;

                if (ImGui::MenuItem("Отладить (F10)") || Input::Button(Input::f10).pressed())
                    debug = 1;

                ImGui::EndMenuBar();
            }

            ImGui::PopStyleVar(1);
            ImGui::PopStyleVar(3);

            if (run || debug)
            {
                SaveChanges();

                ImGui::OpenPopup("gpss_running");

                process_data->output = "";

                auto lambda_stdout = [&](const char *data, size_t size)
                {
                    std::cout << "-";
                    std::scoped_lock lock(process_data->output_mutex);
                    process_data->output.insert(process_data->output.end(), data, data+size);
                };

                process_data->process.emplace("gpssh.exe {}{}"_format(current_file, debug ? " tv" : ""), "", lambda_stdout, lambda_stdout);

                process_data->process_ended = 0;
                process_data->process_running = 1;
                process_data->should_close_popup_automatically = -1;
                process_data->popup_alpha = 0;
                process_data->using_debugger = debug;

                process_data->thread = std::thread([&]
                {
                    process_data->process->get_exit_status();
                    process_data->process_ended = 1;
                    std::cout << "Thread closed.";
                });
                process_data->thread.detach();
            }

            bool proc_ended = process_data->process_ended;
            if (proc_ended)
            {
                process_data->process_ended = 0;
                process_data->process_running = 0;
                LoadOutput();
            }

            clamp_var(process_data->popup_alpha += (ImGui::IsPopupOpen("gpss_running") ? 0.05 : -2), 0.00001, 1); // For some reason ImGui doesn't like alpha == 0, so we use a small number instead.

            if (ImGui::BeginTabBar("tabs"))
            {
                if (ImGui::BeginTabItem("Программа"))
                {
                    ImGui::PushFont(font_mono);
                    if (ImGui::InputTextMultiline("###code_input_", &current_code, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_AllowTabInput))
                        unsaved_changes = 1;

                    bool text_input_focused_now = ImGui::IsItemActive();

                    if (!text_input_focused_now && text_input_focused)
                        FilterCharacters(current_code);

                    text_input_focused = text_input_focused_now;

                    ImGui::PopFont();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Результат", 0, proc_ended * ImGuiTabItemFlags_SetSelected))
                {
                    if (current_output.empty())
                    {
                        ImGui::TextDisabled("%s", "Программа еще не была запущена.");
                    }
                    else
                    {
                        ImGui::PushFont(font_mono);
                        ImGui::InputTextMultiline("###output", current_output.data(), current_output.size(), ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll);
                        ImGui::PopFont();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            if (ImGui::IsPopupOpen("gpss_running"))
            {
                ImGui::SetNextWindowPos(ivec2(modal_margin));
                ImGui::SetNextWindowSize(window.Size() - 2 * modal_margin);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, process_data->popup_alpha);

                if (ImGui::BeginPopupModal("gpss_running", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
                {
                    if (process_data->process_running)
                    {
                        ImGui::TextUnformatted("Работаю...");

                        ImGui::SameLine();

                        if (ImGui::SmallButton("Прервать") || Input::Button(Input::escape).pressed())
                        {
                            process_data->process->kill();
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    else
                    {
                        if (ImGui::SmallButton("Закрыть") || Input::Button(Input::escape).pressed())
                        {
                            process_data->process->kill();
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    if (proc_ended)
                    {
                        if (process_data->should_close_popup_automatically == -1)
                        {
                            process_data->should_close_popup_automatically =
                                process_data->output.find("***") == std::string::npos &&
                                process_data->output.find("Error") == std::string::npos &&
                                process_data->output.find("Warning") == std::string::npos; // I've never seen "Warning" in output, but I want to be extra safe.
                        }

                        if (process_data->should_close_popup_automatically)
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::BeginChildFrame(1, ImGui::GetContentRegionAvail());
                    ImGui::PushFont(font_mono);
                    {
                        std::scoped_lock lock(process_data->output_mutex);
                        ImGui::TextUnformatted(process_data->output.c_str());
                    }
                    ImGui::PopFont();
                    ImGui::EndChildFrame();

                    ImGui::EndPopup();
                }

                ImGui::PopStyleVar();
            }

            ImGui::End();


            // ImGui::ShowDemoWindow();
        }

        void Render() const override
        {
            Graphics::SetClearColor(fvec3(1));
            Graphics::Clear();
        }
    };
}

int main()
{
    { // Initialize
        ImGui::StyleColorsLight();

        ImGui::GetStyle().WindowPadding     = ivec2(8,8);
        ImGui::GetStyle().FramePadding      = ivec2(5,3);
        ImGui::GetStyle().ItemSpacing       = ivec2(8,4);
        ImGui::GetStyle().ItemInnerSpacing  = ivec2(4,4);
        ImGui::GetStyle().TouchExtraPadding = ivec2(0,0);
        ImGui::GetStyle().IndentSpacing     = 20;
        ImGui::GetStyle().ScrollbarSize     = 18;
        ImGui::GetStyle().GrabMinSize       = 12;

        ImGui::GetStyle().WindowBorderSize  = 1;
        ImGui::GetStyle().ChildBorderSize   = 1;
        ImGui::GetStyle().PopupBorderSize   = 1;
        ImGui::GetStyle().FrameBorderSize   = 1;
        ImGui::GetStyle().TabBorderSize     = 0;

        ImGui::GetStyle().WindowRounding    = 3;
        ImGui::GetStyle().ChildRounding     = 2;
        ImGui::GetStyle().FrameRounding     = 3;
        ImGui::GetStyle().PopupRounding     = 2;
        ImGui::GetStyle().ScrollbarRounding = 0;
        ImGui::GetStyle().GrabRounding      = 3;
        ImGui::GetStyle().TabRounding       = 6;

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesDefault());
        builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        font_main = gui_controller.LoadFont("assets/Roboto-Regular.ttf", 16, ImFontConfig{} with(RasterizerFlags = ImGuiFreeType::ForceAutoHint), ranges.Data);
        font_mono = gui_controller.LoadFont("assets/RobotoMono-Regular.ttf", 16, ImFontConfig{} with(RasterizerFlags = ImGuiFreeType::LightHinting), ranges.Data);
        gui_controller.RenderFontsWithFreetype();

        Graphics::Blending::Enable();
        Graphics::Blending::FuncNormalPre();
    }

    States::current_state = Poly::derived<States::Main>;

    Metronome metronome(60);
    Clock::DeltaTimer delta_timer;

    while (1)
    {
        uint64_t delta = delta_timer();
        while (metronome.Tick(delta))
        {
            window.ProcessEvents({gui_controller.EventHook(gui_controller.dont_block_events)});

            if (window.Resized())
                Graphics::Viewport(window.Size());
            if (window.ExitRequested())
                Program::Exit();

            gui_controller.PreTick();
            States::current_state->Tick();
        }

        gui_controller.PreRender();
        States::current_state->Render();
        Graphics::CheckErrors();
        gui_controller.PostRender();

        window.SwapBuffers();
    }
}
