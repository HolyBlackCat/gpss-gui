

const std::string version_string = "1.0.0";
const std::string repo_link = "https://github.com/HolyBlackCat/gpss-gui";

const ivec2 min_screen_size(480, 270);
const std::string base_window_title = "Gpss-gui";
Interface::Window window(base_window_title, min_screen_size * 2, Interface::windowed, Interface::WindowSettings{} with_(min_size = min_screen_size));
Graphics::DummyVertexArray dummy_vao;

const Graphics::ShaderConfig shader_config = Graphics::ShaderConfig::Core();
Interface::ImGuiController gui_controller(Poly::derived<Interface::ImGuiController::GraphicsBackend_Modern>, Interface::ImGuiController::Config{} with_(shader_header = shader_config.common_header, store_state_in_file = ""));

Input::Mouse mouse;

ImFont *font_main = 0;
ImFont *font_mono = 0;

constexpr int modal_margin = 64;
const std::string zero_width_space = "\xEF\xBB\xBF";
constexpr char dir_separator = OnPlatform(WINDOWS)('\\') NotOnPlatform(WINDOWS)('/');

std::string path_prefix;

std::string window_title = base_window_title;

void SetWindowTitle(std::string new_title)
{
    if (window_title == new_title)
        return;

    window_title = std::move(new_title);
    window.SetTitle(window_title);
}

std::string EscapeStringForWidgetName(const std::string &source_str)
{
    std::string ret;

    for (char ch : source_str)
    {
        ret += ch;
        if (ch == '#')
            ret += zero_width_space;
    }

    return ret;
}

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
            int ticks_running = 0;

            std::string output;
            std::mutex output_mutex;
        };

        std::unique_ptr<ProcessData> process_data = std::make_unique<ProcessData>();

        struct Tab
        {
            std::string pretty_name;
            std::string input_file_name;
            std::string output_file_name;
            std::string generic_file_name;
            std::string output;
            bool no_output_file = 1;
            unsigned int id = 0;

            void LoadOutput(bool *ok = 0)
            {
                try
                {
                    output = MemoryFile(output_file_name).string();
                    output.erase(std::remove_if(output.begin(), output.end(), [](char ch){return ch > 0 && ch < ' ' && ch != '\n' && ch != '\t';}), output.end());
                    no_output_file = 0;
                    if (ok)
                        *ok = 1;
                }
                catch (...)
                {
                    output = "";
                    no_output_file = 1;
                    if (ok)
                        *ok = 0;
                    else
                        Interface::MessageBox(Interface::MessageBoxType::warning, "Ошибка", "Не могу прочитать выходной файл `{}`."_format(output_file_name));
                }
            }
        };
        std::vector<Tab> tabs;
        unsigned int tab_counter = 0;
        int active_tab_index = -1;

        std::string gpss_params = "maxcom";

        bool HaveActiveTab()
        {
            return active_tab_index >= 0 && active_tab_index < int(tabs.size());
        }

        bool HaveTabNamed(std::string name)
        {
            return std::find_if(tabs.begin(), tabs.end(), [&](const Tab &tab){return tab.pretty_name == name;}) != tabs.end();
        }

        void AddTab(std::string file_name)
        {
            try
            {
                Filesystem::GetObjectInfo(file_name);
            }
            catch (...)
            {
                Interface::MessageBox(Interface::MessageBoxType::error, "Ошибка", "Не могу прочитать файл `{}`."_format(file_name));
                return;
            }

            std::string file_name_low = file_name;
            for (char &ch : file_name_low)
                if (ch > 0)
                    ch = std::tolower(ch);

            bool is_input;
            if (Strings::EndsWith(file_name_low, ".gps"))
            {
                is_input = 1;
            }
            else if (Strings::EndsWith(file_name_low, ".lis"))
            {
                is_input = 0;
            }
            else
            {
                Interface::MessageBox(Interface::MessageBoxType::error, "Ошибка", "Не могу использовать файл `{}`:\nОжидалось расширение `.gps` или `.lis`."_format(file_name));
                return;
            }

            Tab new_tab;
            new_tab.id = tab_counter++;

            if (auto pos = file_name.find_last_of(dir_separator); pos != std::string::npos)
                new_tab.pretty_name = file_name.substr(pos + 1);
            else
                new_tab.pretty_name = file_name;
            new_tab.pretty_name.resize(new_tab.pretty_name.size() - 4);
            if (HaveTabNamed(new_tab.pretty_name))
            {
                int index = 2;
                std::string new_name;
                while (1)
                {
                    new_name = "{} ({})"_format(new_tab.pretty_name, index);
                    if (!HaveTabNamed(new_name))
                    {
                        new_tab.pretty_name = std::move(new_name);
                        break;
                    }
                    index++;
                }
            }

            new_tab.input_file_name = file_name;
            new_tab.output_file_name = file_name;
            if (is_input)
            {
                new_tab.output_file_name.resize(new_tab.output_file_name.size() - 4);
                new_tab.generic_file_name = new_tab.output_file_name;
                new_tab.output_file_name += ".lis";
            }
            else
            {
                new_tab.input_file_name.resize(new_tab.input_file_name.size() - 4);
                new_tab.generic_file_name = new_tab.input_file_name;
                new_tab.input_file_name += ".gps";
            }

            bool output_ok;
            new_tab.LoadOutput(&output_ok);
            (void)output_ok;

            tabs.push_back(std::move(new_tab));
        }

        void LoadOutput()
        {
            if (!HaveActiveTab())
                return;

            tabs[active_tab_index].LoadOutput();
        }


        void Tick() override
        {
            for (const auto &file_name : window.DroppedFiles())
                AddTab(file_name);

            ImGui::SetNextWindowPos(ivec2(0));
            ImGui::SetNextWindowSize(window.Size());

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ivec2(2,2));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

            ImGui::Begin("main window", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ivec2(4,4));

            bool run = 0;
            bool debug = 0;
            bool about = 0;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Запустить (F9)", nullptr, nullptr, HaveActiveTab()) || Input::Button(Input::f9).pressed())
                    run = 1;

                if (ImGui::MenuItem("Отладить (F10)", nullptr, nullptr, HaveActiveTab()) || Input::Button(Input::f10).pressed())
                    debug = 1;

                if (ImGui::BeginMenu("Настройки"))
                {
                    ImGui::TextUnformatted("Параметры командной строки GPSS");
                    ImGui::InputText("", &gpss_params);
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("О программе"))
                    about = 1;

                ImGui::EndMenuBar();
            }

            ImGui::PopStyleVar(1);
            ImGui::PopStyleVar(3);

            constexpr const char *about_modal_name = "О программе";
            if (about)
                ImGui::OpenPopup(about_modal_name);

            if (ImGui::IsPopupOpen(about_modal_name))
            {
                bool open = 1;
                if (ImGui::BeginPopupModal(about_modal_name, &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::TextUnformatted("GPSS-GUI");
                    ImGui::Spacing();
                    ImGui::TextUnformatted(Str("Версия: ", version_string, "\nСборка от: " __DATE__ ", " __TIME__).c_str());
                    ImGui::Spacing();
                    ImGui::TextUnformatted("Авторы: Михайлов Егор, Холупко Евгений");
                    ImGui::Spacing();
                    ImGui::TextUnformatted(repo_link.c_str());
                    OnPlatform(WINDOWS)
                    (
                        if (ImGui::IsItemHovered())
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                        if (ImGui::IsItemClicked())
                            ShellExecuteA(NULL, "open", repo_link.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    )

                    ImGui::Spacing();
                    if (ImGui::Button("Закрыть"))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }

            { // Tabs
                bool have_active_tab = HaveActiveTab(); // We remember the value now, to avoid 1 frame jitter when closing a tab.

                if (ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoTooltip))
                {
                    for (size_t i = 0; i < tabs.size(); i++)
                    {
                        bool open = 1;

                        if (ImGui::BeginTabItem(Str(EscapeStringForWidgetName(tabs[i].pretty_name), "###", tabs[i].id).c_str(), &open))
                        {
                            SetWindowTitle("{} - {}"_format(base_window_title, tabs[i].generic_file_name));

                            active_tab_index = i;

                            if (tabs[i].no_output_file)
                            {
                                ImGui::TextUnformatted("Программа ещё не была запущена.");
                                ImGui::SameLine();
                                if (ImGui::SmallButton("Запустить"))
                                    run = 1;
                            }
                            else
                            {
                                ImGui::PushFont(font_mono);
                                ImGui::InputTextMultiline("###output", tabs[i].output.data(), tabs[i].output.size(), ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll);
                                ImGui::PopFont();
                            }

                            ImGui::EndTabItem();
                        }

                        if (!open)
                        {
                            tabs.erase(tabs.begin() + i);
                            i--;
                        }
                    }

                    ImGui::EndTabBar();
                }

                if (!have_active_tab)
                {
                    SetWindowTitle(base_window_title);
                    ImGui::TextDisabled("%s", "Перетащите файл в это окно, чтобы открыть его.");
                }
            }

            if ((run || debug) && !process_data->process_running && HaveActiveTab())
            {
                bool can_start = 1;

                try
                {
                    (void)Filesystem::GetObjectInfo(tabs[active_tab_index].input_file_name);
                }
                catch (...)
                {
                    can_start = 0;
                    Interface::MessageBox(Interface::MessageBoxType::error, "Ошибка", "Не найден файл с исходным кодом: `{}`."_format(tabs[active_tab_index].input_file_name));
                }

                std::string gpss_path = path_prefix + "gpssh.exe";

                try
                {
                    (void)Filesystem::GetObjectInfo(gpss_path);
                }
                catch (...)
                {
                    can_start = 0;
                    Interface::MessageBox(Interface::MessageBoxType::error, "Ошибка", "Не найден исполняемый файл: `{}`."_format(gpss_path));
                }

                if (can_start)
                {
                    ImGui::OpenPopup("gpss_running");

                    process_data->output = "";

                    auto lambda_stdout = [&](const char *data, size_t size)
                    {
                        std::scoped_lock lock(process_data->output_mutex);
                        process_data->output.insert(process_data->output.end(), data, data+size);
                    };

                    process_data->process.emplace("{} \"{}\"{} {}"_format(gpss_path, tabs[active_tab_index].input_file_name, debug ? " tv" : "", gpss_params), "", lambda_stdout, lambda_stdout, true);
                    process_data->process->write("Cancel\n");

                    process_data->process_ended = 0;
                    process_data->process_running = 1;
                    process_data->should_close_popup_automatically = -1;
                    process_data->popup_alpha = 0;
                    process_data->using_debugger = debug;
                    process_data->ticks_running = 0;

                    process_data->thread = std::thread([&]
                    {
                        process_data->process->get_exit_status();
                        process_data->process_ended = 1;
                    });
                    process_data->thread.detach();
                }
            }

            bool proc_ended = process_data->process_ended;
            if (proc_ended)
            {
                process_data->process_ended = 0;
                process_data->process_running = 0;
                LoadOutput();
            }

            clamp_var(process_data->popup_alpha += (ImGui::IsPopupOpen("gpss_running") ? 0.05 : -2), 0.00001, 1); // For some reason ImGui doesn't like alpha == 0, so we use a small number instead.

            if (ImGui::IsPopupOpen("gpss_running"))
            {
                process_data->ticks_running++;

                ImGui::SetNextWindowPos(ivec2(modal_margin));
                ImGui::SetNextWindowSize(window.Size() - 2 * modal_margin);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, process_data->popup_alpha);

                if (ImGui::BeginPopupModal("gpss_running", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
                {
                    int status_text_offset_y = 0;

                    // Progress bar
                    if (process_data->process_running)
                    {
                        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, fvec4(0.1,0.5,0.9,1)); // Works as a progress bar color here.
                        ImGui::PushStyleColor(ImGuiCol_Border, fvec3(0).to_vec4(1));
                        ImGui::ProgressBar(sin(process_data->ticks_running / 30.) * 0.45 + 0.5, ivec2(ImGui::GetFrameHeight()), "");
                        ImGui::PopStyleColor(2);
                        ImGui::SameLine();
                    }
                    else
                    {
                        status_text_offset_y = ImGui::GetStyle().FramePadding.y;
                    }

                    std::string close_button_text = process_data->process_running ? "Прервать" : "Закрыть";

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + status_text_offset_y);
                    ImGui::TextUnformatted(process_data->process_running ? "Работаю..." : "Готово");
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(close_button_text.c_str()).x - ImGui::GetStyle().FramePadding.x * 2);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - status_text_offset_y);

                    if (ImGui::Button(close_button_text.c_str()) || Input::Button(Input::escape).pressed())
                    {
                        if (process_data->process_running)
                            process_data->process->kill();
                        ImGui::CloseCurrentPopup();
                    }

                    if (proc_ended)
                    {
                        if (process_data->should_close_popup_automatically == -1)
                        {
                            process_data->should_close_popup_automatically =
                                process_data->output.find("***") == std::string::npos &&
                                process_data->output.find("Error") == std::string::npos &&
                                process_data->output.find("Warning") == std::string::npos && // I've never seen "Warning" in output, but I want to be extra safe.
                                process_data->output.find("invalid option") == std::string::npos;
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

int main(int argc, char **argv)
{
    // Get installation directory
    if (argc >= 1)
    {
        std::string command = argv[0];
        if (auto pos = command.find_last_of(dir_separator); pos != std::string::npos)
            path_prefix = command.substr(0, pos) + dir_separator;
    }

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
        builder.AddText(zero_width_space.c_str());
        ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        font_main = gui_controller.LoadFont(path_prefix + "Roboto-Regular.ttf", 16, ImFontConfig{} with(RasterizerFlags = ImGuiFreeType::ForceAutoHint), ranges.Data);
        font_mono = gui_controller.LoadFont(path_prefix + "RobotoMono-Regular.ttf", 16, ImFontConfig{} with(RasterizerFlags = ImGuiFreeType::LightHinting), ranges.Data);
        gui_controller.RenderFontsWithFreetype();

        Graphics::Blending::Enable();
        Graphics::Blending::FuncNormalPre();

        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    }

    auto &new_state = States::current_state.assign<States::Main>();
    for (int i = 1; i < argc; i++)
        new_state.AddTab(argv[i]);

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

        double frame_len = Clock::TicksToSeconds(Clock::Time() - delta_timer.LastTimePoint());
        uint64_t target_frame_len = 1.0 / metronome.Frequency();
        if (target_frame_len > frame_len)
            Clock::WaitSeconds((target_frame_len - frame_len) * 0.9);
    }
}
