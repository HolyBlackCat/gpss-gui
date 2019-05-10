const ivec2 screen_size(480, 270);
Interface::Window window("Delta", screen_size * 2, Interface::windowed, Interface::WindowSettings{} with_(min_size = screen_size));
Graphics::DummyVertexArray dummy_vao;

const Graphics::ShaderConfig shader_config = Graphics::ShaderConfig::Core();
Interface::ImGuiController gui_controller(Poly::derived<Interface::ImGuiController::GraphicsBackend_Modern>, Interface::ImGuiController::Config{} with_(shader_header = shader_config.common_header));

Input::Mouse mouse;

ImFont *font_main;
ImFont *font_mono;

std::string text = &1[R"(
Student GPSS/H Release 3.70 (PR221)      6 May 2019   22:01:45     File: 4.gps

    Line# Stmt#  If Do  Block#  *Loc    Operation  A,B,C,D,E,F,G    Comments

        1     1                         SIMULATE
        2     2                         INITIAL    X$NNN,15
        3     3              1          GENERATE   ,,,1,10
        4     4              2    LAB1  TEST L     CH$BUF,X$NNN
        5     5              3          ADVANCE    10
        6     6              4          SPLIT      1,LAB1
        7     7              5          LINK       BUF,FIFO
        8     8
        9     9              6          GENERATE   ,,,1,5
       10    10              7    LAB2  TEST G     CH$BUF,0
       11    11              8          UNLINK     BUF,LAB3,1
       12    12              9          TERMINATE
       13    13
       14    14             10    LAB3  SEIZE      CPU
       15    15             11          ADVANCE    25,5
       16    16             12          RELEASE    CPU
       17    17             13          TRANSFER   .02,LAB5,LAB4
       18    18             14    LAB4  UNLINK     BUF,LAB6,ALL
       19    19             15    LAB5  PRIORITY   5
       20    20             16          TRANSFER   ,LAB2
       21    21
       22    22             17    LAB6  TERMINATE
       23    23
       24    24             18          GENERATE   4000
       25    25             19          TERMINATE  1
       26    26                         START      1
       27    27                         END

Entity Dictionary (in ascending order by entity number; "*" => value conflict.)

       Facilities: 1=CPU

 Fullword Savexes: 1=NNN

      User Chains: 1=BUF

Symbol   Value   EQU Defns  Context      References by Statement Number

LAB1         2           4  Block            6
LAB2         7          10  Block           20
LAB3        10          14  Block           11
LAB4        14          18  Block           17
LAB5        15          19  Block           17
LAB6        17          22  Block           18

CPU          1              Facility        14    16

NNN          1              Fullword Svx     2     4

BUF          1              User Chain       4     7    10    11    18



Storage Requirements (Bytes)

Compiled Code:     1040
Compiled Data:      140
Miscellaneous:        0
Entities:           376
Common:           32720   (Set via MAXCOM option -- can vary with model size)
-----------------------
Total:            34276



GPSS/H Model Size:

Control Statements      4
Blocks                 19



Simulation begins.

Relative Clock: 4000.0000   Absolute Clock: 4000.0000



Block Current     Total  Block Current     Total
1                     1  11          1       158
LAB1                218  12                  157
3           1       218  13                  157
4                   434  LAB4                  3
5          14       217  LAB5                157
6                     1  16                  157
LAB2                158  LAB6                 45
8                   158  18                    1
9                   158  19                    1
LAB3                158



          --Avg-Util-During--
Facility  Total  Avail  Unavl     Entries    Average   Current  Percent  Seizing  Preempting
           Time   Time   Time               Time/Xact   Status   Avail     Xact      Xact
     CPU  0.990                       158      25.063    AVAIL              205



User Chain   Entries    Average      Average     Current     Maximum
                       Time/Xact    Contents    Contents    Contents
     BUF         217     233.349      12.659          14          15



Non-zero Fullword Savevalues:  (NAME : VALUE)

     NNN:          15



  Random    Antithetic     Initial     Current      Sample   Chi-Square
  Stream      Variates    Position    Position       Count   Uniformity
       1           OFF      100000      100315         315      0.97



Status of Common Storage

   30456 bytes available
    2264 in use
    2376 used (max)

Simulation complete.  Absolute Clock: 4000.0000



Total Block Executions: 2557

Blocks / second:        12049123

Microseconds / Block:   0.08



Elapsed Time Used (Sec)

Pass1:           0.00
Sym/Xref         0.00
Pass2:           0.00
Load/Ctrl:       0.00
Execution:       0.00
Output:          0.00
---------------------
Total:           0.00

)"];

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
        void Tick() override
        {
            ImGui::SetNextWindowPos(ivec2(0));
            ImGui::SetNextWindowSize(window.Size());

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ivec2(2,1));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

            ImGui::Begin("main window", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ivec2(4,4));

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Файл"))
                {
                    ImGui::MenuItem("Открыть");

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::PopStyleVar(1);
            ImGui::PopStyleVar(3);

            ImGui::Button("Запустить");
            ImGui::SameLine();
            ImGui::Button("Отладчик");

            ImGui::PushFont(font_mono);

            ImGui::InputTextMultiline("###output", text.data(), text.size(), ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll);

            ImGui::PopFont();

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
        font_mono = gui_controller.LoadFont("assets/RobotoMono-Regular.ttf", 16, ImFontConfig{} with(RasterizerFlags = 0), ranges.Data);
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
            window.ProcessEvents({gui_controller.EventHook()});

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
