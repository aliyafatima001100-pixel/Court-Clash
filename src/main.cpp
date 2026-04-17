// ============================================================
// FINAL TENNIS GAME – All Bugs Fixed
// ============================================================

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"

using namespace std;
int g_humanPlayer = 1; // 0=Sinner, 1=Carlos, 2=Medvedev
int g_cpuPlayer   = 0;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
Texture2D menuBackground;

static const char* PLAYER_NAMES[3] = {"Jannik Sinner", "Carlos Alcaraz", "Daniil Medvedev"};
static const char* SHORT_NAMES[3]  = {"Sinner", "Carlos", "Medvedev"};
static const int   WORLD_RANKS[3]  = {2, 1, 10};

struct ClipInfo { 
    const char* folder;     // not used anymore (can be empty)
    const char* videoPath;  // path to .mpg file
    const char* audioPath;  // path to .mp3 file
    int frames;             // not used (kept for compatibility)
};

static const ClipInfo CHAMP_WIN_CLIPS[3][3] = {
    { {"", "", "",0}, 
      {"", "D:/oop/sinner_wins_carlos.mpg", "D:/oop/sinner_wins_carlos.mp3", 0}, 
      {"", "D:/oop/sinner_wins_med.mpg",    "D:/oop/sinner_wins_med.mp3",    0} },
    { {"", "D:/oop/carlos_wins_sinner.mpg", "D:/oop/carlos_wins_sinner.mp3", 0}, 
      {"", "", "",0}, 
      {"", "D:/oop/carlos_wins_med.mpg",    "D:/oop/carlos_wins_med.mp3",    0} },
    { {"", "D:/oop/med_wins_sinner.mpg",    "D:/oop/med_wins_sinner.mp3",    0}, 
      {"", "D:/oop/med_wins_carlos.mpg",    "D:/oop/med_wins_carlos.mp3",    0}, 
      {"", "", "",0} },
};

static const ClipInfo SET_WIN_CLIPS[3][3] = {
    { {"", "", "", 0}, 
      {"", "D:/oop/sinner_set.mpg", "D:/oop/sinner_set.mp3", 0}, 
      {"", "D:/oop/sinner_set.mpg", "D:/oop/sinner_set.mp3", 0} },
    { {"", "D:/oop/carlos_set.mpg", "D:/oop/carlos_set.mp3", 0}, 
      {"", "", "", 0}, 
      {"", "D:/oop/carlos_set.mpg", "D:/oop/carlos_set.mp3", 0} },
    { {"", "D:/oop/med_set.mpg", "D:/oop/med_set.mp3", 0}, 
      {"", "D:/oop/med_set.mpg", "D:/oop/med_set.mp3", 0}, 
      {"", "", "", 0} },
};

// ============================================================
// Scene base
// ============================================================
class Scene {
public:
    virtual void update() = 0;
    virtual void draw()   = 0;
    virtual ~Scene() {}
};
Scene* currentScene = nullptr;
void changeScene(Scene* newScene) {
    if (currentScene) delete currentScene;
    currentScene = newScene;
}

// Forward declarations
class MenuScene;
class TennisGameScene;
class MemoryGameScene;
class RulesScene;
class MatchupScene;
class PlayerSelectScene;

// ============================================================
// PLAYER SELECT SCENE
// ============================================================
class PlayerSelectScene : public Scene {
    int selected = 1;
    float timer = 0;
    float fadeOut = 0;
    bool fading = false;
    Texture2D bg;
    float hoverScale[3] = {1, 1, 1};

public:
    PlayerSelectScene() {
        bg = LoadTexture("D:/oop/menu_page.png");
    }
    ~PlayerSelectScene() {
        UnloadTexture(bg);
    }

    void update() override;

    void draw() override {
        DrawTexturePro(bg, {0,0,(float)bg.width,(float)bg.height},
            {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT}, {0,0}, 0, WHITE);
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.5f));

        float fadeAlpha = fading ? Clamp(1.0f - fadeOut/0.6f, 0.0f, 1.0f) : 1.0f;
        const char* title = "SELECT YOUR PLAYER";
        int tw = MeasureText(title, 54);
        DrawText(title, (WINDOW_WIDTH-tw)/2, 60, 54, Fade(GOLD, fadeAlpha));

        float cardW = 280, cardH = 140;
        float totalW = 3 * cardW + 2 * 60;
        float startX = (WINDOW_WIDTH - totalW) / 2;
        float cardY = WINDOW_HEIGHT/2 - cardH/2 - 20;

        const char* playerNames[3] = {"Jannik Sinner", "Carlos Alcaraz", "Daniil Medvedev"};
        const char* playerRanks[3] = {"World #2", "World #1", "World #10"};
        Color playerColors[3] = {SKYBLUE, GREEN, ORANGE};

        for (int i = 0; i < 3; i++) {
            float cx = startX + i * (cardW + 60) + cardW/2;
            float s = hoverScale[i];
            float cw = cardW * s, ch = cardH * s;
            float cx0 = cx - cw/2, cy0 = cardY + (cardH - ch)/2;

            bool isSel = (i == selected);
            Color borderColor = isSel ? GOLD : Fade(WHITE, 0.4f);
            Color fillColor = isSel ? Fade(playerColors[i], 0.25f) : Fade(WHITE, 0.08f);

            DrawRectangleRounded({cx0, cy0, cw, ch}, 0.15f, 8, fillColor);
            DrawRectangleRoundedLines({cx0, cy0, cw, ch}, 0.15f, 8, borderColor);

            int nameSize = isSel ? 34 : 28;
            int nameWidth = MeasureText(playerNames[i], nameSize);
            DrawText(playerNames[i], (int)(cx - nameWidth/2), (int)(cy0 + 20), nameSize, Fade(WHITE, fadeAlpha));

            int rankSize = 22;
            int rankWidth = MeasureText(playerRanks[i], rankSize);
            DrawText(playerRanks[i], (int)(cx - rankWidth/2), (int)(cy0 + ch - 44), rankSize, Fade(playerColors[i], fadeAlpha));

            if (isSel) {
                DrawTriangle({cx-14, cy0+ch+20}, {cx+14, cy0+ch+20}, {cx, cy0+ch+6},
                             Fade(GOLD, fadeAlpha*0.9f));
            }
        }

        const char* hint = "SPACE / ENTER to confirm";
        int hw = MeasureText(hint, 24);
        DrawText(hint, (WINDOW_WIDTH-hw)/2, WINDOW_HEIGHT-70, 24, Fade(LIGHTGRAY, fadeAlpha*0.8f));
    }
};

// ============================================================
// MENU SCENE
// ============================================================
class MenuScene : public Scene {
public:
    void update() override {
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) ||
            IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            changeScene(new PlayerSelectScene());
        }
    }
    void draw() override {
        DrawTexturePro(menuBackground, {0,0,(float)menuBackground.width,(float)menuBackground.height},
                       {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT}, {0,0}, 0, WHITE);
        DrawText("TENNIS CHAMPIONSHIP", WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 50, 40, GOLD);
        DrawText("PRESS SPACE TO START", WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 50, 20, RAYWHITE);
    }
};

// ============================================================
// VideoClipScene – Image sequence player (STUTTER-PROOF)
// ============================================================
class VideoClipScene : public Scene {
public:
    using NextFn = std::function<Scene*()>;

    NextFn nextFn;
    int    winnerIdx;
    int    loserIdx;
    bool   isFinal;

    plm_t* plm        = nullptr;
    Texture2D videoTex   = {0};
    float     fallbackTimer = 0;

    Music  music;
    bool   musicLoaded   = false;

    std::vector<unsigned char> frameBuffer;
    bool frameReady = false; // ADDED: Flag to track if a new frame needs uploading

    // Callback that receives each decoded video frame
    static void onVideoFrame(plm_t* /*plm*/, plm_frame_t* frame, void* user) {
        auto* self = (VideoClipScene*)user;
        if (!self->videoTex.id) return;
        
        int w = frame->width, h = frame->height;
        if (self->frameBuffer.size() != w * h * 3) {
            self->frameBuffer.resize(w * h * 3);
        }
        
        // ONLY decode to RAM here. Do NOT upload to the GPU yet!
        plm_frame_to_rgb(frame, self->frameBuffer.data(), w * 3);
        self->frameReady = true; 
    }

    VideoClipScene(const ClipInfo& clip, NextFn next,
                   int winner = -1, int loser = -1, bool final = false)
        : nextFn(next), winnerIdx(winner), loserIdx(loser), isFinal(final)
    {
        if (clip.videoPath && clip.videoPath[0] != '\0') {
            plm = plm_create_with_filename(clip.videoPath);
            if (plm) {
                plm_set_loop(plm, 0);
                plm_set_audio_enabled(plm, 0); 
                plm_set_video_decode_callback(plm, onVideoFrame, this);
                
                int w = plm_get_width(plm);
                int h = plm_get_height(plm);
                
                Image img = GenImageColor(w, h, BLACK);
                ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8); 
                videoTex = LoadTextureFromImage(img);
                UnloadImage(img);
            } else {
                TraceLog(LOG_ERROR, "Failed to open video: %s", clip.videoPath);
            }
        }

        if (clip.audioPath && clip.audioPath[0] != '\0') {
            music = LoadMusicStream(clip.audioPath);
            if (music.stream.buffer != nullptr) {
                PlayMusicStream(music);
                musicLoaded = true;
            }
        }
    }

    ~VideoClipScene() {
        if (plm) plm_destroy(plm);
        if (videoTex.id != 0) UnloadTexture(videoTex);
        if (musicLoaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
        }
    }

    void update() override {
        if (musicLoaded) UpdateMusicStream(music);

        float dt = GetFrameTime();
        if (dt > 0.033f) dt = 0.033f;
        fallbackTimer += dt;

        if (plm) {
            // Reset the flag before decoding
            frameReady = false; 
            
            // This might trigger the callback 0, 1, or multiple times
            plm_decode(plm, dt); 
            
            // Upload to GPU EXACTLY ONCE per frame, breaking the lag spiral!
            if (frameReady) {
                UpdateTexture(videoTex, frameBuffer.data());
            }

            if (plm_has_ended(plm) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                changeScene(nextFn());
            }
        } else {
            if (fallbackTimer >= 4.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                changeScene(nextFn());
            }
        }
    }

    void draw() override {
        ClearBackground(BLACK);

        if (plm && videoTex.id != 0) {
            DrawTexturePro(videoTex,
                {0, 0, (float)videoTex.width, (float)videoTex.height},
                {0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT},
                {0, 0}, 0, WHITE);
        } else if (!plm) {
            const char* msg = "Press SPACE to continue";
            int mw = MeasureText(msg, 32);
            DrawText(msg, (WINDOW_WIDTH - mw) / 2, WINDOW_HEIGHT / 2 + 60, 32, Fade(WHITE, 0.7f));
        }

        if (winnerIdx >= 0 && isFinal) {   // Only show for championship (final) clips
    bool humanWon = (winnerIdx == g_humanPlayer);
    string line1 = humanWon ? "CONGRATULATIONS! You Win!" : "You Lost. Better luck next time!";
    string line2 = humanWon
        ? (string(SHORT_NAMES[winnerIdx]) + " defeats " + SHORT_NAMES[loserIdx])
        : (string(SHORT_NAMES[winnerIdx]) + " defeats " + SHORT_NAMES[loserIdx]);
    int s1 = 52, w1 = MeasureText(line1.c_str(), s1);
    int s2 = 32, w2 = MeasureText(line2.c_str(), s2);
    DrawRectangle(0, WINDOW_HEIGHT - 130, WINDOW_WIDTH, 130, Fade(BLACK, 0.75f));
    DrawText(line1.c_str(), (WINDOW_WIDTH-w1)/2, WINDOW_HEIGHT - 110,
             s1, humanWon ? GOLD : LIGHTGRAY);
    DrawText(line2.c_str(), (WINDOW_WIDTH-w2)/2, WINDOW_HEIGHT - 50, s2, WHITE);
}

        const char* skip = "SPACE to skip";
        DrawText(skip, WINDOW_WIDTH - 160, WINDOW_HEIGHT - 30, 18, Fade(WHITE, 0.5f));
    }
};
// ============================================================
// Card (memory game)
// ============================================================
class Card {
public:
    Texture2D front, back;
    Vector2 position;
    float cardW=260, cardH=360;
    bool isFlipped=false, isMatched=false;
    float scaleX=1.0f;
    bool isAnimating=false, showFront=false;
    int id;

    Rectangle getRect(){ return {position.x, position.y, cardW, cardH}; }
    bool isClicked(Vector2 m){ return CheckCollisionPointRec(m, getRect()); }

    void flip(){
        if (!isAnimating && !isMatched) { isFlipped = !isFlipped; isAnimating = true; }
    }

    void update(){
        if (!isAnimating) return;
        if (!showFront == isFlipped) {
            scaleX -= 0.06f;
            if (scaleX <= 0) { scaleX = 0; showFront = isFlipped; }
        } else {
            scaleX += 0.06f;
            if (scaleX >= 1) { scaleX = 1; isAnimating = false; }
        }
    }

    void draw(){
        Texture2D tex = showFront ? front : back;
        float drawW = cardW * scaleX;
        float offX = (cardW - drawW) / 2.0f;
        DrawTexturePro(tex,
            {0, 0, (float)tex.width, (float)tex.height},
            {position.x + offX, position.y, drawW, cardH},
            {0, 0}, 0, WHITE);
    }
};

// ============================================================
// PAUSE SCENE – simple message + wait for SPACE
// ============================================================
class PauseScene : public Scene {
    string message;
    using NextFn = Scene* (*)();
    NextFn nextSceneFn;
public:
    PauseScene(const string& msg, NextFn next) : message(msg), nextSceneFn(next) {}
    void update() override {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            changeScene(nextSceneFn());
        }
    }
    void draw() override {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Color{64, 64, 64, 255});  
        int lineHeight = 42;
        // Split message by newline
        vector<string> lines;
        size_t pos = 0;
        string s = message;
        while ((pos = s.find('\n')) != string::npos) {
            lines.push_back(s.substr(0, pos));
            s.erase(0, pos + 1);
        }
        lines.push_back(s);
        float y = WINDOW_HEIGHT / 2 - (lines.size() * lineHeight) / 2;
        for (const auto& line : lines) {
            int w = MeasureText(line.c_str(), 34);
            DrawText(line.c_str(), (WINDOW_WIDTH - w) / 2, (int)y, 34, YELLOW);
            y += lineHeight;
        }
        // Small hint
        const char* hint = "SPACE / ENTER to continue";
        int hw = MeasureText(hint, 22);
        DrawText(hint, (WINDOW_WIDTH - hw) / 2, WINDOW_HEIGHT - 60, 22, Fade(WHITE, 0.7f));
    }
};

// ============================================================
// TENNIS GAME SCENE
// ============================================================
class TennisGameScene : public Scene {
private:
    float courtX, courtY, courtW, courtH;
    Texture2D courtTexture;

    const float F_LEFT=0.110f, F_RIGHT=0.888f, F_TOP=0.198f;
    const float F_SERVICE_TOP=0.340f, F_NET=0.455f;
    const float F_SERVICE_BOT=0.692f, F_BOTTOM=0.839f;

    float pLeft, pRight, pTop, pBottom, pSinglesLeft, pSinglesRight;
    float pServiceTop, pServiceBot, pNet;

    Texture2D runFrames[4];
    Texture2D idleFront, idleBack, servePose;
    Texture2D hitRun, hitIdleFront, hitIdleBack, hitServe;
    static constexpr float PLAYER_DRAW_H = 105.0f;

    enum MoveState  { MOVE_IDLE, MOVE_RUN };
    enum ServePhase { SERVE_NONE, SERVE_WAIT, SERVE_TOSS };

    MoveState  p1Move=MOVE_IDLE;  ServePhase p1Serve=SERVE_WAIT;
    float p1SwingTimer=0; float runFrameTimer=0; int runFrameIdx=0;

    MoveState  p2Move=MOVE_IDLE;  ServePhase p2Serve=SERVE_NONE;
    float p2SwingTimer=0; float cpuRunFrameTimer=0; int cpuRunFrameIdx=0;
    float cpuServeTimer=0;
    float serveCooldown=0.0f;

    enum CpuTactic { CPU_BASELINE, CPU_APPROACH_NET, CPU_AT_NET, CPU_RETREAT, CPU_APPROACH_SLOW };
    CpuTactic cpuTactic=CPU_BASELINE;
    float cpuTacticTimer=0, cpuDropShotCooldown=999.0f;
    float cpuSlowApproachTimer=0.0f;

    int   cpuVolleyCount=0;
    static constexpr int CPU_VOLLEY_LIMIT=4;

    struct Player { float x=0, y=0, prevX=0, prevY=0; bool flipX=false; };
    Player p1, p2;

    bool  p1TossVisible=false; float p1TossZ=0, p1TossVZ=0;
    bool  p2TossVisible=false; float p2TossZ=0, p2TossVZ=0;

    Vector2 ballPos={0,0};
    float   ballZ=0, ballVZ=0, ballRadius=9.0f;
    Vector2 ballVel={0,0};
    bool    ballInPlay=false;
    int     lastHitter=0, bouncesOnSide=0;
    bool    ballOnP1Side=true, ballBounced=false;
    float   ballDrag=0, ballSpin=0;

    static const int TRAIL_LEN=12;
    Vector2 trailPos[TRAIL_LEN];
    int trailHead=0; float trailTimer=0;
    static constexpr float TRAIL_INTERVAL=0.016f;

    bool    bounceFlash=false;
    float   bounceFlashTimer=0;
    Vector2 bounceFlashPos={0,0};

    enum ShotType { SHOT_NORMAL, SHOT_LOB, SHOT_DROP, SHOT_SMASH, SHOT_SERVE };
    ShotType lastShotType=SHOT_NORMAL;

    bool  smashReady         = false;
    float smashCooldownTimer = 0.0f;
    int   rallyHitCount      = 0;
    static constexpr float SMASH_COOLDOWN           = 10.0f;
    static constexpr float SMASH_PER_BOUNCE_CHANCE  = 0.18f;

    bool  smashMiniActive    = false;
    float smashMiniTimer     = 0.0f;
    int   smashSpacePresses  = 0;
    static constexpr float SMASH_MINI_DURATION  = 2.2f;
    static constexpr int   SMASH_SPACE_REQUIRED = 3;
    static constexpr float SMASH_SLOW_FACTOR    = 0.25f;

    const float GRAVITY = 620.0f;

    // FIX: Points now go 0-1-2-3-4 where 4 = win (displayed as 0/15/30/40/W)
    // We use 0,1,2,3 displayed as 0,15,30,40 and winning condition is pts>=4 OR deuce logic
    int p1Points=0, p2Points=0, p1Sets=0, p2Sets=0;
    bool deuce=false, advantage=false; int advantagePlayer=0;
    bool pointOver=false; float pointTimer=0;
    int server=1, serveCount=0;
    bool gameOver=false; int winner=0; float gameOverTimer=0;

    string statusMsg="Press SPACE to serve"; float statusTimer=0;
    const int GAMES_TO_WIN=6, SETS_TO_WIN=2;
    mt19937 rng;

    bool  serveBarActive  = false;
    float serveBarPos     = 0.0f;
    float serveBarDir     = 1.0f;
    float serveBarSpeed   = 0.75f;
    static constexpr float SERVE_ZONE_CENTER = 0.50f;
    static constexpr float SERVE_ZONE_HALF   = 0.13f;
    static constexpr float SERVE_BULL_HALF   = 0.045f;
    int   serveBarFaults  = 0;
    bool  isFaultServeInFlight = false;
    bool  isDoubleFault   = false;
    int   smashTriggerCount = 6;
    bool  smashTriggeredThisRally = false;

    // ---- helpers ----
    // FIX: pointStr maps 0->0, 1->15, 2->30, 3->40 (no index 4 displayed, win is handled separately)
    const char* pointStr(int p){
        if(p==0) return "0";
        if(p==1) return "15";
        if(p==2) return "30";
        return "40"; // 3 and above show 40 (deuce logic handles the rest)
    }

    // FIX: athleteIndex correctly maps side to the assigned player index
    // p1 (side 1) = human player, p2 (side 2) = cpu player
    int athleteIndex(int side) { return (side==1) ? g_humanPlayer : g_cpuPlayer; }

    void setStatus(const string& m, float d=2.5f){ statusMsg=m; statusTimer=d; }

    void clearTrail(){
        for(int i=0;i<TRAIL_LEN;i++) trailPos[i]={-9999,-9999};
        trailHead=0; trailTimer=0;
    }

    void initCourt(){
        courtH=WINDOW_HEIGHT*0.9f; courtW=courtH*1.3f;
        courtX=(WINDOW_WIDTH-courtW)/2.0f; courtY=WINDOW_HEIGHT*0.05f;
        pLeft=courtX+courtW*F_LEFT; pRight=courtX+courtW*F_RIGHT;
        pSinglesLeft=pLeft+(pRight-pLeft)*0.12f;
        pSinglesRight=pRight-(pRight-pLeft)*0.12f;
        pTop=courtY+courtH*F_TOP; pBottom=courtY+courtH*F_BOTTOM;
        pServiceTop=courtY+courtH*F_SERVICE_TOP;
        pServiceBot=courtY+courtH*F_SERVICE_BOT;
        pNet=courtY+courtH*F_NET;
    }

    void resetPositionsForServe(){
        int tot=p1Points+p2Points; bool dc=(tot%2==0);
        float cx=(pLeft+pRight)/2.0f, off=courtW*0.18f;
        p1.y=pNet-300.0f; p2.y=pNet+460.0f;
        if(server==1){ p1.x=dc?cx-off:cx+off; p2.x=dc?cx+off:cx-off; }
        else          { p2.x=dc?cx+off:cx-off; p1.x=dc?cx-off:cx+off; }
        p1.flipX=p2.flipX=false;
    }

    void resetSmashMini(){
        smashMiniActive=false; smashMiniTimer=0; smashSpacePresses=0; smashReady=false;
    }

    void resetPlayerForServe(){
        int tot=p1Points+p2Points;
        bool dc=(tot%2==0);
        float cx=(pLeft+pRight)/2.0f;
        float off=courtW*0.18f;
        if(server==1){
            p1.y=pNet-300.0f;
            p1.x=dc?cx-off:cx+off;
        } else {
            p2.y=pNet+460.0f;
            p2.x=dc?cx+off:cx-off;
        }
        p1.flipX=false; p2.flipX=false;
    }

    void resetForServe(){
        resetPositionsForServe();
        p1TossVisible=p2TossVisible=false;
        if(server==1){ p1Serve=SERVE_WAIT; p2Serve=SERVE_NONE; }
        else          { p1Serve=SERVE_NONE; p2Serve=SERVE_WAIT; }
        p1Move=p2Move=MOVE_IDLE;
        p1SwingTimer=p2SwingTimer=0;
        runFrameIdx=cpuRunFrameIdx=0;
        runFrameTimer=cpuRunFrameTimer=0;
        rallyHitCount=0; smashCooldownTimer=0; resetSmashMini();
        smashTriggeredThisRally=false;
        lastShotType=SHOT_SERVE;
        cpuTactic=CPU_BASELINE; cpuTacticTimer=0;
        cpuDropShotCooldown=999.0f; cpuVolleyCount=0;
        ballDrag=ballSpin=0; ballBounced=false; ballInPlay=false;
        clearTrail(); bounceFlash=false;
        serveBarActive=false; serveBarPos=0; serveBarDir=1;
        isFaultServeInFlight=false; isDoubleFault=false;
        setStatus(server==1 ? "Press SPACE to serve" : "CPU serving...");
    }

    void launchBall(float fromX, float fromY, float fromZ,
                    float toX, float toY, float travelSpeed,
                    float lobFactor, float drag, float spin, int hitter)
    {
        lastHitter=hitter; bouncesOnSide=0; ballBounced=false; ballInPlay=true;
        clearTrail();
        toX=Clamp(toX, pSinglesLeft+55, pSinglesRight-55);
        if(hitter==1) toY=Clamp(toY, pNet+75, pBottom+200);
        else          toY=Clamp(toY, pTop-200, pNet-75);
        ballPos={fromX, fromY};
        ballOnP1Side=(fromY<pNet);
        float dy=toY-fromY;
        float timeToReach=fabs(dy)/travelSpeed;
        if(timeToReach<0.10f) timeToReach=0.10f;
        ballVel.x=(toX-fromX)/timeToReach;
        ballVel.y=dy/timeToReach;
        float maxPeak=30.0f+lobFactor*130.0f;
        if(maxPeak<fromZ) maxPeak=fromZ;
        float ballVZ_land=0.5f*GRAVITY*timeToReach-fromZ/timeToReach;
        float peakFromLand=fromZ+ballVZ_land*ballVZ_land/(2.0f*GRAVITY);
        if(peakFromLand<maxPeak){
            float vz_for_peak=sqrtf(2.0f*GRAVITY*(maxPeak-fromZ));
            float T_land=(vz_for_peak+sqrtf(vz_for_peak*vz_for_peak+2.0f*GRAVITY*fromZ))/GRAVITY;
            if(T_land>0.05f){ ballVel.x=(toX-fromX)/T_land; ballVel.y=dy/T_land; }
            ballVZ=vz_for_peak;
        } else {
            float cappedPeak=fminf(peakFromLand, maxPeak);
            float vz_capped=sqrtf(2.0f*GRAVITY*(cappedPeak-fromZ));
            float T_land=(vz_capped+sqrtf(vz_capped*vz_capped+2.0f*GRAVITY*fromZ))/GRAVITY;
            if(T_land>0.05f){ ballVel.x=(toX-fromX)/T_land; ballVel.y=dy/T_land; }
            ballVZ=vz_capped;
        }
        ballZ=fromZ; ballDrag=drag; ballSpin=spin;
    }

    void serve(int srv, float tossX, float tossY, float tossZ){
        float cx=(pSinglesLeft+pSinglesRight)/2.0f;
        int tot=p1Points+p2Points;
        bool deuceCourt=(tot%2==0);
        float targetX, targetY;
        if(srv==1){
            targetY=pNet+(pServiceBot-pNet)*0.55f;
            targetX=deuceCourt?cx+courtW*0.09f:cx-courtW*0.09f;
        } else {
            targetY=pNet-(pNet-pServiceTop)*0.55f;
            targetX=deuceCourt?cx-courtW*0.09f:cx+courtW*0.09f;
        }
        targetX=Clamp(targetX, pSinglesLeft+50, pSinglesRight-50);
        launchBall(tossX,tossY,tossZ,targetX,targetY,700.0f,0.12f,0.05f,0.2f,srv);
    }

    void launchFaultServe(){
        isFaultServeInFlight=true;
        p2Move=MOVE_IDLE; cpuTactic=CPU_BASELINE;
        cpuRunFrameIdx=0; cpuRunFrameTimer=0; cpuVolleyCount=0;
        float tossX=p1.x+(p1.flipX?-30:30);
        float tossY=p1.y-PLAYER_DRAW_H*0.7f;
        float tossZ=180.0f;
        float cx=(pSinglesLeft+pSinglesRight)/2.0f;
        int tot=p1Points+p2Points;
        bool dc=(tot%2==0);
        float targetX=dc?cx+courtW*0.09f:cx-courtW*0.09f;
        float targetY=pServiceBot+(pBottom-pServiceBot)*0.75f;
        targetX=Clamp(targetX, pSinglesLeft+50, pSinglesRight-50);
        launchBall(tossX,tossY,tossZ,targetX,targetY,680.0f,0.07f,0.03f,0.25f,1);
    }

    void p1HitShot(){
        p1SwingTimer=0.45f;
        float cx=(pSinglesLeft+pSinglesRight)/2.0f;
        float targetX=(p2.x>cx)?pSinglesLeft+90:pSinglesRight-90;
        float targetY=pBottom-160.0f;
        float speed=380.0f, lob=0.45f, drag=0.14f, spin=0.2f;
        ShotType stype=SHOT_NORMAL;
        bool pL=IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_A);
        bool pR=IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D);
        bool pU=IsKeyDown(KEY_UP)||IsKeyDown(KEY_W);
        bool pD=IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_S);
        if(pU){
            targetY=pBottom+50.0f;
            if(pL&&!pR) targetX=pSinglesLeft+70.0f;
            if(pR&&!pL) targetX=pSinglesRight-70.0f;
            speed=480.0f; lob=0.72f; drag=0.10f; spin=0.55f;
            stype=SHOT_LOB;
            
        } else if(pD){
            targetY=pNet+80.0f;
            speed=240.0f; lob=0.55f; drag=0.35f; spin=-0.35f;
            stype=SHOT_DROP;
        
        }
        if(stype!=SHOT_SMASH){
            if(pL&&!pU) targetX=pSinglesLeft+70;
            if(pR&&!pU) targetX=pSinglesRight-70;
        }
        rallyHitCount++;
        lastShotType=stype;
        float originZ=fmaxf(ballZ,5.0f);
        launchBall(ballPos.x,ballPos.y,originZ,targetX,targetY,speed,lob,drag,spin,1);
    }

    void p1FireSmash(){
        p1SwingTimer=0.45f;
        float cx=(pSinglesLeft+pSinglesRight)/2.0f;
        float targetX=(p2.x>cx)?pSinglesLeft+60:pSinglesRight-60;
        float targetY=pNet+120.0f;
        float speed=720.0f, lob=0.03f, drag=0.03f, spin=0.8f;
        setStatus("SMASH!!!", 1.0f);
        smashCooldownTimer=SMASH_COOLDOWN;
        resetSmashMini();
        lastShotType=SHOT_SMASH;
        rallyHitCount++;
        float originZ=fmaxf(ballZ,5.0f);
        launchBall(ballPos.x,ballPos.y,originZ,targetX,targetY,speed,lob,drag,spin,1);
    }

    void cpuHitShot(){
        p2SwingTimer=0.35f;
        float cx=(pSinglesLeft+pSinglesRight)/2.0f;
        float targetX=(p1.x>cx)?pSinglesLeft+90:pSinglesRight-90;
        float targetY=pTop+150.0f;
        float speed=450.0f, lob=0.45f, drag=0.15f, spin=0.2f;
        ShotType stype=SHOT_NORMAL;
        float distToNet=fabs(p2.y-pNet);
        uniform_int_distribution<int> roll(0,9);
        int r=roll(rng);
        bool atNet=(cpuTactic==CPU_AT_NET || distToNet<130.0f);
        if(atNet){
            cpuVolleyCount++;
            if(cpuVolleyCount>=CPU_VOLLEY_LIMIT){
                targetY=pTop+80.0f; speed=270.0f; lob=0.95f; drag=0.10f; spin=0.15f;
                stype=SHOT_LOB; cpuTactic=CPU_RETREAT; cpuVolleyCount=0;
                setStatus("CPU retreats!", 0.5f);
            } else {
                if(rng()%2==0) targetX=(p1.x>cx)?pSinglesLeft+90:pSinglesRight-90;
                else { targetX=p1.x; targetX=Clamp(targetX, pSinglesLeft+70, pSinglesRight-70); }
                targetY=pTop+180.0f; speed=470.0f; lob=0.14f; drag=0.07f; spin=0.4f;
                stype=SHOT_SMASH;
            }
        } else if(cpuTactic==CPU_APPROACH_SLOW){
            targetY=pTop+200.0f; speed=480.0f; lob=0.22f; drag=0.09f; spin=0.3f;
            stype=SHOT_NORMAL;
            setStatus("CPU Approach", 0.3f);
        } else if(r<=1 && cpuDropShotCooldown>8.0f){
            targetY=pNet-45.0f; speed=180.0f; lob=0.45f; drag=0.55f; spin=-0.45f;
            stype=SHOT_DROP; cpuDropShotCooldown=0;
            uniform_int_distribution<int> ar(0,1);
            if(ar(rng)==0){ cpuTactic=CPU_APPROACH_SLOW; cpuSlowApproachTimer=1.8f; }
            else cpuTactic=CPU_BASELINE;
        } else if(r==2){
            targetY=pNet-(pNet-pTop)*0.92f; speed=290.0f; lob=0.92f; drag=0.10f; spin=0.2f;
            stype=SHOT_LOB;
        } else {
            uniform_real_distribution<float> xV(pSinglesLeft+60, pSinglesRight-60);
            uniform_real_distribution<float> yV(pTop+80, pNet-90);
            targetX=xV(rng); targetY=yV(rng);
            speed=300.0f+(float)(r*12);
            lob=0.38f+r*0.025f; drag=0.15f; spin=0.2f; stype=SHOT_NORMAL;
        }
        lastShotType=stype;
        float originZ=fmaxf(ballZ,5.0f);
        launchBall(ballPos.x,ballPos.y,originZ,targetX,targetY,speed,lob,drag,spin,2);
        rallyHitCount++;
    }

    // ----------------------------------------------------------------
    // FIX: scorePoint — correct tennis scoring
    // Points: 0,1,2,3 = 0,15,30,40. Win a game when pts>=4 and diff>=2,
    // OR via deuce/advantage logic when both reach 3.
    // ----------------------------------------------------------------
void scorePoint(int scorer) {
        ballInPlay = false;
        p1TossVisible = false;
        p2TossVisible = false;
        cpuServeTimer = 0;
        clearTrail();
        bounceFlash = false;
        serveBarActive = false;
        serveBarFaults = 0;
        isFaultServeInFlight = false;
        resetSmashMini();

        string scorerName;
        if (scorer == 1) {
            scorerName = SHORT_NAMES[g_humanPlayer];
        } else {
            scorerName = SHORT_NAMES[g_cpuPlayer];
        }

        auto winGame = [&]() {
    // Alternate server for next game
    if (server == 1) server = 2;
    else server = 1;

    // Increment set count for the scorer
    if (scorer == 1) p1Sets++;
    else p2Sets++;

    // Reset game points and deuce/advantage
    p1Points = 0;
    p2Points = 0;
    deuce = false;
    advantage = false;
    advantagePlayer = 0;

    int currentSets = (scorer == 1) ? p1Sets : p2Sets;

    // --- MATCH WIN? ---
    if (currentSets >= SETS_TO_WIN) {
        int winnerAth = (scorer == 1) ? g_humanPlayer : g_cpuPlayer;
        int loserAth  = (scorer == 1) ? g_cpuPlayer : g_humanPlayer;
        const ClipInfo& champClip = CHAMP_WIN_CLIPS[winnerAth][loserAth];
        changeScene(new VideoClipScene(champClip,
            []()->Scene*{ return new MenuScene(); },
            winnerAth, loserAth, true));
        return;
    }

    // --- SET WIN (but not match) ---
    // Save current state so we can resume after the pause
    TennisGameScene::resumeData = {
        p1Points, p2Points,
        p1Sets,   p2Sets,
        server,   serveCount,
        deuce,    advantage,
        advantagePlayer,
        pointOver, pointTimer
    };
    TennisGameScene::resumeValid = true;

    int winnerAth = (scorer == 1) ? g_humanPlayer : g_cpuPlayer;
    int loserAth  = (scorer == 1) ? g_cpuPlayer : g_humanPlayer;
    const ClipInfo& setClip = SET_WIN_CLIPS[winnerAth][loserAth];

    // Build the message: "Next set begins – CPU serves" (or human if server==1)
    string msg = "Next set begins. ";
    if (server == 2) msg += "CPU serves.\n";
    else msg += "You serve.\n";
    msg += "Press SPACE to continue";

    changeScene(new VideoClipScene(setClip,
        [msg]()->Scene* {
            return new PauseScene(msg, []()->Scene* {
                return new TennisGameScene(true);
            });
        },
        winnerAth, loserAth, false));
    return;
};

        if (deuce == false) {
            if (scorer == 1) {
                p1Points++;
            } else {
                p2Points++;
            }

            if (p1Points == 3 && p2Points == 3) {
                deuce = true;
                advantage = false;
                advantagePlayer = 0;
                setStatus("Deuce");
            } else if (scorer == 1 && p1Points >= 4 && (p1Points - p2Points) >= 2) {
                winGame();
                return;
            } else if (scorer == 2 && p2Points >= 4 && (p2Points - p1Points) >= 2) {
                winGame();
                return;
            } else if (scorer == 1 && p1Points == 4 && p2Points < 3) {
                winGame();
                return;
            } else if (scorer == 2 && p2Points == 4 && p1Points < 3) {
                winGame();
                return;
            }
        } else {
            if (advantage == true) {
                if (advantagePlayer == scorer) {
                    winGame();
                    return;
                } else {
                    advantage = false;
                    advantagePlayer = 0;
                    setStatus("Deuce");
                }
            } else {
                advantage = true;
                advantagePlayer = scorer;
                setStatus("Advantage -- " + scorerName);
            }
        }
        pointOver = true;
        pointTimer = 0;
    }

    void movePlayer(Player& pl, Vector2 tgt, float dt, bool ai,
                    float minY, float maxY, float speedOverride=-1.0f)
    {
        float spd = (speedOverride>0) ? speedOverride : (ai ? 355.0f : 700.0f);
        pl.prevX=pl.x; pl.prevY=pl.y;
        Vector2 d={tgt.x-pl.x, tgt.y-pl.y};
        float dist=sqrtf(d.x*d.x+d.y*d.y);
        if(dist>1.0f){
            if(ai){ pl.x+=(d.x/dist)*fminf(spd*dt,dist); pl.y+=(d.y/dist)*fminf(spd*dt,dist); }
            else  { pl.x=tgt.x; pl.y=tgt.y; }
        }
        pl.x=Clamp(pl.x, 0, (float)WINDOW_WIDTH);
        pl.y=Clamp(pl.y, minY, maxY);
        if(pl.x<pl.prevX) pl.flipX=true;
        else if(pl.x>pl.prevX) pl.flipX=false;
    }

    void drawPlayer(const Player& pl, MoveState mv, ServePhase srv, float swing, bool cpu, int ri){
        bool serving=(srv==SERVE_WAIT||srv==SERVE_TOSS);
        bool hitting=(swing>0);
        Texture2D tex;
        if(cpu) tex=(mv==MOVE_RUN)?(hitting?hitIdleFront:runFrames[ri]):(hitting?hitIdleFront:idleFront);
        else{
            if(serving) tex=hitting?hitServe:servePose;
            else if(mv==MOVE_RUN) tex=hitting?hitRun:runFrames[ri];
            else tex=hitting?hitServe:servePose;
        }
        float asp=(float)tex.width/tex.height;
        float dH=PLAYER_DRAW_H, dW=dH*asp;
        float sW=pl.flipX?-(float)tex.width:(float)tex.width;
        float sX=pl.flipX?(float)tex.width:0.0f;
        DrawTexturePro(tex,{sX,0,sW,(float)tex.height},{pl.x-dW*0.5f,pl.y-dH,dW,dH},{0,0},0,WHITE);
    }

    void drawBall(){
        if(!ballInPlay) return;
        for(int i=0;i<TRAIL_LEN;i++){
            int idx=(trailHead-1-i+TRAIL_LEN)%TRAIL_LEN;
            Vector2 tp=trailPos[idx];
            if(tp.x<-9000) continue;
            float age=(float)(i+1)/TRAIL_LEN;
            float alpha=Clamp(0.50f*(1.0f-age),0,1.0f);
            float r=ballRadius*(1.0f-age*0.55f);
            Color c={(unsigned char)255,(unsigned char)(200-(int)(age*160)),0,(unsigned char)(int)(alpha*255)};
            DrawCircleV(tp,r,c);
        }
        float sAlpha=Clamp(0.5f-(ballZ/500.0f),0.07f,0.5f);
        float sScale=Clamp(1.0f+(ballZ/200.0f),1.0f,2.0f);
        DrawEllipse((int)ballPos.x,(int)ballPos.y,ballRadius*1.3f*sScale,ballRadius*0.42f*sScale,Fade(BLACK,sAlpha));
        Vector2 dp={ballPos.x,ballPos.y-ballZ};
        if(bounceFlash){
            float prog=1.0f-bounceFlashTimer/0.18f;
            float fr=ballRadius*(1.5f+prog*2.5f);
            DrawCircleLines((int)bounceFlashPos.x,(int)bounceFlashPos.y,fr,Fade(WHITE,0.65f*(1.0f-prog)));
        }
        DrawCircleV(dp,ballRadius,YELLOW);
        DrawCircleLines(dp.x,dp.y,ballRadius,Fade(ORANGE,0.8f));
        DrawCircleV({dp.x-3,dp.y-3},3,Fade(WHITE,0.6f));
        static float seam=0; seam+=0.10f;
        DrawLineEx({dp.x+cosf(seam)*ballRadius*0.7f,dp.y+sinf(seam)*ballRadius*0.7f},
                   {dp.x-cosf(seam)*ballRadius*0.7f,dp.y-sinf(seam)*ballRadius*0.7f},2,Fade(ORANGE,0.7f));
    }

    void drawCourt(){
        float eH=courtH*0.12f;
        DrawTexturePro(courtTexture,{0,0,(float)courtTexture.width,(float)courtTexture.height},
            {courtX,courtY-eH*0.5f,courtW,courtH+eH},{0,0},0,WHITE);
    }

    void drawScoreboard(){
        float px=WINDOW_WIDTH/2.0f-340, py=4, w=680, h=66;
        DrawRectangleRounded({px,py,w,h},0.15f,8,Fade(BLACK,0.85f));
        DrawRectangleRoundedLines({px,py,w,h},0.15f,8,WHITE);

        // FIX: p1 is always the human, p2 is always the CPU
        // Labels use the correct player names based on g_humanPlayer and g_cpuPlayer
        string p1Label=string(SHORT_NAMES[g_humanPlayer])+" (YOU)";
        string p2Label=string(SHORT_NAMES[g_cpuPlayer])+" (CPU)";
        DrawText(p1Label.c_str(),(int)(px+14),(int)(py+8),18,GREEN);
        DrawText(p2Label.c_str(),(int)(px+14),(int)(py+36),18,BLUE);

        DrawText("Sets",(int)(px+200),(int)(py+2),13,LIGHTGRAY);
        DrawText(TextFormat("%d",p1Sets),(int)(px+204),(int)(py+18),24,WHITE);
        DrawText(TextFormat("%d",p2Sets),(int)(px+204),(int)(py+40),24,WHITE);

        DrawText("Points",(int)(px+280),(int)(py+2),13,LIGHTGRAY);
        if(deuce){
            if(advantage){
                if(advantagePlayer==1) DrawText("ADV",(int)(px+292),(int)(py+18),24,YELLOW);
                else DrawText("ADV",(int)(px+292),(int)(py+40),24,YELLOW);
            } else DrawText("DEUCE",(int)(px+278),(int)(py+24),20,YELLOW);
        } else {
            DrawText(pointStr(p1Points),(int)(px+292),(int)(py+18),24,WHITE);
            DrawText(pointStr(p2Points),(int)(px+292),(int)(py+40),24,WHITE);
        }

        DrawText("Serve",(int)(px+380),(int)(py+2),13,LIGHTGRAY);
        float pointerX=px+380;
        float pointerY=(server==1)?py+28:py+46;
        Vector2 arrow[3]={{pointerX,pointerY},{pointerX-10,pointerY-6},{pointerX-10,pointerY+6}};
        DrawTriangle(arrow[0],arrow[1],arrow[2],YELLOW);


        // Small labels directly on the court so you know who is who
        const char* youLabel = "YOU (TOP)";
        const char* cpuLabel = "CPU (BOTTOM)";
        DrawText(youLabel, (int)(courtX + courtW*0.02f), (int)(pTop - 40), 20, Fade(GREEN, 0.85f));
        DrawText(cpuLabel, (int)(courtX + courtW*0.02f), (int)(pBottom + 40), 20, Fade(RED, 0.85f));
    }

    void drawStatusMsg(){
        if(statusTimer<=0) return;
        int sz=34, w=MeasureText(statusMsg.c_str(),sz);
        float my=pNet-30;
        DrawRectangle((WINDOW_WIDTH-w)/2-14,(int)my-4,w+28,sz+8,Fade(BLACK,0.65f));
        DrawText(statusMsg.c_str(),(WINDOW_WIDTH-w)/2,(int)my,sz,YELLOW);
    }

    void drawServeBar(){
        if(!serveBarActive) return;
        const float BAR_W=36.0f, BAR_H=240.0f;
        const float BAR_X=72.0f;
        const float BAR_Y=WINDOW_HEIGHT*0.5f-BAR_H*0.5f;
        DrawRectangleRounded({BAR_X-4,BAR_Y-4,BAR_W+8,BAR_H+8},0.3f,8,Fade(BLACK,0.75f));
        DrawRectangle((int)BAR_X,(int)BAR_Y,(int)BAR_W,(int)BAR_H,{180,30,30,210});
        float bullTop=BAR_Y+(SERVE_ZONE_CENTER-SERVE_BULL_HALF)*BAR_H;
        float bullBot=BAR_Y+(SERVE_ZONE_CENTER+SERVE_BULL_HALF)*BAR_H;
        DrawRectangle((int)BAR_X,(int)bullTop,(int)BAR_W,(int)(bullBot-bullTop),GOLD);
        DrawRectangleLinesEx({BAR_X,bullTop,BAR_W,bullBot-bullTop},1,Fade(WHITE,0.9f));
        float ptrY=Clamp(BAR_Y+serveBarPos*BAR_H,BAR_Y,BAR_Y+BAR_H);
        bool inBull=(fabs(serveBarPos-SERVE_ZONE_CENTER)<=SERVE_BULL_HALF);
        Color ptrColor=inBull?GOLD:RED;
        DrawTriangle({BAR_X-16.0f,ptrY-9.0f},{BAR_X-16.0f,ptrY+9.0f},{BAR_X+1.0f,ptrY},ptrColor);
        const char* title="SERVE";
        int titleW=MeasureText(title,16);
        DrawText(title,(int)(BAR_X+BAR_W*0.5f-titleW*0.5f),(int)(BAR_Y-26),16,WHITE);
    }

    void drawSmashMini(){
        if(!smashMiniActive) return;
        float progress=smashMiniTimer/SMASH_MINI_DURATION;
        float pulse=0.9f+0.2f*sinf(GetTime()*12.0f);
        float panelX=40.0f, panelY=WINDOW_HEIGHT/2.0f-90.0f, panelW=320.0f, panelH=140.0f;
        float scale=1.0f+(pulse-0.9f)*0.5f;
        float scaledW=panelW*scale, scaledH=panelH*scale;
        float scaledX=panelX-(scaledW-panelW)/2.0f, scaledY=panelY-(scaledH-panelH)/2.0f;
        DrawRectangleRounded({scaledX,scaledY,scaledW,scaledH},0.2f,8,Fade(BLACK,0.85f));
        DrawRectangleRoundedLinesEx({scaledX,scaledY,scaledW,scaledH},0.2f,8,3.0f,GOLD);
        const char* instr="PRESS SPACE X3";
        int fontSize=32, iw=MeasureText(instr,fontSize);
        DrawText(instr,(int)(scaledX+scaledW/2-iw/2),(int)(scaledY+28),fontSize,YELLOW);
        float dotR=12.0f, dotSpacing=48.0f;
        float dotStartX=scaledX+scaledW/2-(SMASH_SPACE_REQUIRED-1)*dotSpacing/2.0f;
        float dotY=scaledY+88;
        for(int i=0;i<SMASH_SPACE_REQUIRED;i++){
            float dx=dotStartX+i*dotSpacing;
            if(i<smashSpacePresses){ DrawCircleV({dx,dotY},dotR,GOLD); DrawCircleLines(dx,dotY,dotR,WHITE); }
            else{ DrawCircleV({dx,dotY},dotR,Fade(GRAY,0.5f)); DrawCircleLines(dx,dotY,dotR,LIGHTGRAY); }
        }
        float timerBarW=panelW*0.8f, timerBarH=16.0f;
        float timerBarX=panelX+(panelW-timerBarW)/2.0f, timerBarY=panelY+panelH+20.0f;
        DrawRectangleRounded({timerBarX,timerBarY,timerBarW,timerBarH},0.3f,6,Fade(BLACK,0.7f));
        Color barCol=(progress>0.5f)?GREEN:((progress>0.25f)?ORANGE:RED);
        DrawRectangleRounded({timerBarX,timerBarY,timerBarW*progress,timerBarH},0.3f,6,barCol);
        char timerText[32]; sprintf(timerText,"%.1f sec",smashMiniTimer);
        int tw=MeasureText(timerText,16);
        DrawText(timerText,(int)(timerBarX+timerBarW/2-tw/2),(int)(timerBarY+timerBarH+8),16,YELLOW);
    }

public:
    struct ResumeData {
        int p1Points, p2Points;
        int p1Sets,   p2Sets;
        int server,   serveCount;
        bool deuce,   advantage;
        int advantagePlayer;
        bool pointOver;
        float pointTimer;
    };
    static ResumeData resumeData;
    static bool       resumeValid;

    TennisGameScene(bool resume=false) : rng(time(nullptr)){
        runFrames[0]=LoadTexture("sprite_0.png");
        runFrames[1]=LoadTexture("sprite_1.png");
        runFrames[2]=LoadTexture("sprite_2.png");
        runFrames[3]=LoadTexture("sprite_3.png");
        idleFront   =LoadTexture("sprite_4.png");
        idleBack    =LoadTexture("sprite_6.png");
        servePose   =LoadTexture("sprite_5.png");
        hitRun      =LoadTexture("sprite_3_hit.png");
        hitIdleFront=LoadTexture("sprite_4_hit.png");
        hitIdleBack =LoadTexture("sprite_6_hit.png");
        hitServe    =LoadTexture("sprite_5_hit.png");
        courtTexture=LoadTexture("D:/oop/tennis_court.png");
        clearTrail(); initCourt();

        if(resume && resumeValid){
            p1Points=resumeData.p1Points; p2Points=resumeData.p2Points;
            p1Sets  =resumeData.p1Sets;   p2Sets  =resumeData.p2Sets;
            server  =resumeData.server;   serveCount=resumeData.serveCount;
            deuce   =resumeData.deuce;    advantage=resumeData.advantage;
            advantagePlayer=resumeData.advantagePlayer;
            pointOver=resumeData.pointOver; pointTimer=resumeData.pointTimer;
            resetPositionsForServe();
            p1TossVisible=p2TossVisible=false;
            if(server==1){ p1Serve=SERVE_WAIT; p2Serve=SERVE_NONE; }
            else          { p1Serve=SERVE_NONE; p2Serve=SERVE_WAIT; }
            p1Move=p2Move=MOVE_IDLE;
            p1SwingTimer=p2SwingTimer=0;
            runFrameIdx=cpuRunFrameIdx=0;
            runFrameTimer=cpuRunFrameTimer=0;
            rallyHitCount=0; smashCooldownTimer=0; resetSmashMini();
            smashTriggeredThisRally=false;
            lastShotType=SHOT_SERVE;
            cpuTactic=CPU_BASELINE; cpuTacticTimer=0;
            cpuDropShotCooldown=999.0f; cpuVolleyCount=0;
            ballDrag=ballSpin=0; ballBounced=false; ballInPlay=false;
            clearTrail(); bounceFlash=false;
            serveBarActive=false; serveBarPos=0; serveBarDir=1;
            isFaultServeInFlight=false; isDoubleFault=false;
            setStatus(server==1?"Press SPACE to serve":"CPU serving...");
            resumeValid = false;  // consume the resume data
        } else {
            resetForServe();
        }
    }

    ~TennisGameScene(){
        for(int i=0;i<4;i++) UnloadTexture(runFrames[i]);
        UnloadTexture(idleFront); UnloadTexture(idleBack); UnloadTexture(servePose);
        UnloadTexture(hitRun); UnloadTexture(hitIdleFront); UnloadTexture(hitIdleBack);
        UnloadTexture(hitServe); UnloadTexture(courtTexture);
    }

    void update() override {
        float rawDt = GetFrameTime();
        if(rawDt > 0.05f) rawDt = 0.05f;

        float dt = smashMiniActive ? rawDt * SMASH_SLOW_FACTOR : rawDt;

        if(statusTimer > 0)       statusTimer      -= rawDt;
        if(bounceFlashTimer > 0){ bounceFlashTimer -= dt; if(bounceFlashTimer<=0) bounceFlash=false; }
        if(smashCooldownTimer > 0.0f) smashCooldownTimer -= rawDt;

        if(gameOver){
            gameOverTimer += dt;
            if(gameOverTimer > 1.5f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                changeScene(new MenuScene());
            }
            return;
        }

        if(pointOver){
            pointTimer += dt;
            if(pointTimer > 2.2f){
                pointOver=false;
                resetForServe();
            }
            return;
        }

        // SMASH MINI-GAME
        if(smashMiniActive){
            smashMiniTimer -= rawDt;

            if(IsKeyPressed(KEY_SPACE)){
                smashSpacePresses++;
                if(smashSpacePresses >= SMASH_SPACE_REQUIRED){
                    p1FireSmash();
                    return;
                }
            }

            if(smashMiniTimer <= 0.0f){
                setStatus("Smash missed! Point lost.", 2.0f);
                resetSmashMini();
                scorePoint(2);
                return;
            }
        }

        if(p1SwingTimer>0) p1SwingTimer=max(0.0f,p1SwingTimer-dt);
        if(p2SwingTimer>0) p2SwingTimer=max(0.0f,p2SwingTimer-dt);
        if(serveCooldown>0) serveCooldown-=dt;
        if(cpuSlowApproachTimer>0) cpuSlowApproachTimer-=dt;
        cpuDropShotCooldown+=dt;
        cpuTacticTimer+=dt;

        if(ballInPlay){
            trailTimer+=dt;
            if(trailTimer>=TRAIL_INTERVAL){
                trailTimer=0;
                trailPos[trailHead]={ballPos.x, ballPos.y-ballZ};
                trailHead=(trailHead+1)%TRAIL_LEN;
            }
        }

        // P1 SERVE
        if(!ballInPlay && server==1 && !smashMiniActive){
            if(p1Serve==SERVE_WAIT){
                p1TossVisible=true; p1TossZ=0;

                if(!serveBarActive && IsKeyPressed(KEY_SPACE)){
                    serveBarActive=true;
                    serveBarPos=0.0f; serveBarDir=1.0f;
                    setStatus("Hit SPACE on the gold zone!", 3.0f);
                }
                else if(serveBarActive){
                    serveBarPos += serveBarDir * serveBarSpeed * dt;
                    if(serveBarPos>=1.0f){ serveBarPos=1.0f; serveBarDir=-1.0f; }
                    if(serveBarPos<=0.0f){ serveBarPos=0.0f; serveBarDir= 1.0f; }

                    if(IsKeyPressed(KEY_SPACE)){
                        serveBarActive=false;
                        float diff=fabs(serveBarPos-SERVE_ZONE_CENTER);
                        bool good=(diff<=SERVE_BULL_HALF);

                        if(good){
                            serveBarFaults=0;
                            setStatus("Correct serve!", 1.2f);
                            p1SwingTimer=0.45f;
                            p1TossVisible=false;
                            p1Serve=SERVE_NONE;
                            float tossX=p1.x+(p1.flipX?-30:30);
                            float tossY=p1.y-PLAYER_DRAW_H*0.7f;
                            serve(1, tossX, tossY, 180.0f);
                            serveCooldown=0.2f;
                            serveCount++;
                        } else {
                            serveBarFaults++;
                            serveCount++;
                            if(serveBarFaults>=2){
                                serveBarFaults=0;
                                p1SwingTimer=0.35f;
                                p1TossVisible=false;
                                p1Serve=SERVE_NONE;
                                setStatus("Double Fault! Point to CPU.", 2.5f);
                                isDoubleFault=true;
                                launchFaultServe();
                            } else {
                                setStatus("FAULT! Second serve...", 2.2f);
                                p1SwingTimer=0.45f;
                                p1TossVisible=false;
                                p1Serve=SERVE_WAIT;
                                launchFaultServe();
                            }
                        }
                    }
                }
            }
            else if(p1Serve==SERVE_TOSS){
                p1TossZ  += p1TossVZ * dt;
                p1TossVZ -= GRAVITY  * dt;
                if(p1TossZ<0){
                    p1Serve=SERVE_WAIT; p1TossZ=0; p1TossVZ=0;
                    setStatus("Press SPACE to serve");
                }
            }
        }

        // CPU SERVE
        if(!ballInPlay && server==2){
            cpuServeTimer+=dt;
            if(p2Serve==SERVE_WAIT){
                p2TossVisible=true; p2TossZ=0;
                if(cpuServeTimer>0.8f){
                    p2Serve=SERVE_TOSS; p2TossVZ=450;
                    cpuServeTimer=0;
                }
            }
            else if(p2Serve==SERVE_TOSS){
                p2TossZ  += p2TossVZ * dt;
                p2TossVZ -= GRAVITY  * dt;
                if(p2TossVZ<0 && p2TossZ>60){
                    serveCount++;
                    p2SwingTimer=0.45f;
                    p2TossVisible=false;
                    p2Serve=SERVE_NONE;
                    float tossX=p2.x+(p2.flipX?-30:30);
                    float tossY=p2.y-PLAYER_DRAW_H*0.7f;
                    float tossZ=p2TossZ;
                    serve(2, tossX, tossY, tossZ);
                    setStatus("CPU Serves", 1.0f);
                    cpuServeTimer=0;
                }
                if(p2TossZ<0){
                    p2Serve=SERVE_WAIT; p2TossZ=0; p2TossVZ=0; cpuServeTimer=0;
                }
            }
        }

        // P1 MOVEMENT
        bool serveInProgress=(!ballInPlay && (server==1||server==2));
        bool spacePressed=IsKeyDown(KEY_SPACE);
        bool holdA=IsKeyDown(KEY_A)    ||IsKeyDown(KEY_LEFT);
        bool holdD=IsKeyDown(KEY_D)    ||IsKeyDown(KEY_RIGHT);
        bool holdW=IsKeyDown(KEY_W)    ||IsKeyDown(KEY_UP);
        bool holdS=IsKeyDown(KEY_S)    ||IsKeyDown(KEY_DOWN);
        bool shotCombo=spacePressed&&(holdA||holdD||holdW||holdS)&&!serveInProgress&&ballInPlay;

        bool p1Moved=false;
        if(!serveInProgress && !serveBarActive && !smashMiniActive && !isFaultServeInFlight){
            if(IsKeyDown(KEY_W)){ p1.y-=540.0f*dt; p1Moved=true; }
            if(IsKeyDown(KEY_S)){ p1.y+=540.0f*dt; p1Moved=true; }
            if(IsKeyDown(KEY_A)){ p1.x-=540.0f*dt; p1Moved=true; p1.flipX=true; }
            if(IsKeyDown(KEY_D)){ p1.x+=540.0f*dt; p1Moved=true; p1.flipX=false; }
            if(!shotCombo){
                if(IsKeyDown(KEY_LEFT)) { p1.x-=370.0f*dt; p1Moved=true; p1.flipX=true; }
                if(IsKeyDown(KEY_RIGHT)){ p1.x+=370.0f*dt; p1Moved=true; p1.flipX=false; }
                if(IsKeyDown(KEY_UP))   { p1.y-=370.0f*dt; p1Moved=true; }
                if(IsKeyDown(KEY_DOWN)) { p1.y+=370.0f*dt; p1Moved=true; }
            }
        }
        p1.x=Clamp(p1.x, 0, (float)WINDOW_WIDTH);
        p1.y=Clamp(p1.y, 0, pNet-20.0f);

        // P1 HIT
        if(!serveInProgress && spacePressed && ballInPlay && serveCooldown<=0.0f && !smashMiniActive){
            bool onP1Side=(ballPos.y < pNet+25.0f);
            bool notMyBall=(lastHitter!=1);
            float visBallY=ballPos.y-ballZ;
            float hitDist=Vector2Distance({ballPos.x,visBallY},{p1.x,p1.y-PLAYER_DRAW_H*0.45f});
            float maxHitZ=ballBounced?68.0f:55.0f;
            bool normalHit=onP1Side && notMyBall && hitDist<120.0f && ballZ<maxHitZ && bouncesOnSide<=1;
            if(normalHit){
                p1HitShot();
            }
        }

        p1Move=p1Moved?MOVE_RUN:MOVE_IDLE;
        if(p1Move==MOVE_RUN){ runFrameTimer+=dt; if(runFrameTimer>=0.10f){ runFrameTimer=0; runFrameIdx=(runFrameIdx+1)%4; } }
        else{ runFrameIdx=0; runFrameTimer=0; }

        // CPU TACTIC TRANSITIONS
        if(ballInPlay && cpuTacticTimer>1.2f){
            cpuTacticTimer=0;
            if(cpuTactic==CPU_APPROACH_NET && fabs(p2.y-pNet)<120)
                cpuTactic=CPU_AT_NET;
            if(cpuTactic==CPU_AT_NET && ballPos.y<pNet-200)
                cpuTactic=CPU_RETREAT;
            if(cpuTactic==CPU_RETREAT && p2.y>pNet+280){
                cpuTactic=CPU_BASELINE; cpuVolleyCount=0;
            }
            if(cpuTactic==CPU_APPROACH_SLOW && cpuSlowApproachTimer<=0.0f)
                cpuTactic=CPU_AT_NET;
        }

        if(ballInPlay && ballVel.y>0 && lastHitter==1){
            if(lastShotType==SHOT_LOB &&
               (cpuTactic==CPU_AT_NET||cpuTactic==CPU_APPROACH_NET||cpuTactic==CPU_APPROACH_SLOW)){
                cpuTactic=CPU_RETREAT;
                cpuTacticTimer=0.0f;
                
            }
        }

        // CPU MOVEMENT
        if(ballInPlay && p2Serve==SERVE_NONE && p1Serve==SERVE_NONE && !isFaultServeInFlight && !isDoubleFault){
            Vector2 cpuTgt={p2.x,p2.y};
            float cx2=(pSinglesLeft+pSinglesRight)/2.0f;

            if(cpuTactic==CPU_AT_NET||cpuTactic==CPU_APPROACH_NET){
                cpuTgt={ballVel.y>0?ballPos.x:cx2, pNet+150.0f};
            } else if(cpuTactic==CPU_APPROACH_SLOW){
                cpuTgt={ballVel.y>0?ballPos.x:cx2, pNet+150.0f};
            } else if(cpuTactic==CPU_RETREAT){
                cpuTgt={cx2, pBottom-80.0f};
            } else {
                if(ballVel.y>0){
                    cpuTgt.x=ballPos.x;
                    cpuTgt.y=Clamp(ballPos.y+ballVel.y*0.25f, pNet+40.0f, pBottom);
                } else {
                    cpuTgt={cx2, pBottom-55.0f};
                }
            }

            float px2=p2.x, py2=p2.y;
            float moveSpd=(cpuTactic==CPU_APPROACH_SLOW)?200.0f:-1.0f;
            movePlayer(p2, cpuTgt, dt, true, pNet+100.0f, pBottom+20.0f, moveSpd);

            bool cpuMoved=fabs(p2.x-px2)>0.5f||fabs(p2.y-py2)>0.5f;
            p2Move=cpuMoved?MOVE_RUN:MOVE_IDLE;
            if(p2.x<p2.prevX) p2.flipX=true;
            else if(p2.x>p2.prevX) p2.flipX=false;
        }
        if(p2Move==MOVE_RUN){ cpuRunFrameTimer+=dt; if(cpuRunFrameTimer>=0.10f){ cpuRunFrameTimer=0; cpuRunFrameIdx=(cpuRunFrameIdx+1)%4; } }
        else{ cpuRunFrameIdx=0; cpuRunFrameTimer=0; }

        // BALL PHYSICS
        if(ballInPlay){
            float spdX=fabs(ballVel.x);
            float dragX=1.0f-(ballDrag*(1.0f+spdX/500.0f))*dt;
            dragX=Clamp(dragX,0.0f,1.0f);
            ballVel.x*=dragX;

            float dragY=1.0f-ballDrag*0.25f*dt;
            dragY=Clamp(dragY,0.0f,1.0f);
            ballVel.y*=dragY;

            float grav=GRAVITY+ballSpin*160.0f;
            ballVZ-=grav*dt;

            ballPos.x+=ballVel.x*dt;
            ballPos.y+=ballVel.y*dt;
            ballZ    +=ballVZ    *dt;

            // BOUNCE
            if(ballZ<=0.0f && ballVZ<0.0f){
                ballZ=0.0f;
                bounceFlash=true; bounceFlashTimer=0.18f; bounceFlashPos=ballPos;

                float rest=(ballSpin>0)?Clamp(0.68f+ballSpin*0.08f,0.20f,0.80f)
                                       :Clamp(0.52f+ballSpin*0.14f,0.18f,0.80f);
                ballVZ=fabs(ballVZ)*rest;

                float hd;
                if(lastShotType==SHOT_SMASH) hd=0.90f;
                else hd=(ballSpin<0)?0.40f:0.78f;
                ballVel.x*=hd; ballVel.y*=hd;

               // OUT CHECK
                bool out=(ballPos.x<pSinglesLeft||ballPos.x>pSinglesRight||
                          ballPos.y<pTop||ballPos.y>pBottom);
                if(out){
                    if(isFaultServeInFlight){
                        isFaultServeInFlight=false;
                        if(isDoubleFault){
                            isDoubleFault=false;
                            if (lastHitter == 1) {
                                scorePoint(2);
                            } else {
                                scorePoint(1);
                            }
                            setStatus("Double Fault!", 1.2f);
                        } else {
                            serveBarActive=false; serveBarPos=0.0f; serveBarDir=1.0f;
                            resetPlayerForServe();
                            p2Move=MOVE_IDLE; cpuRunFrameIdx=0; cpuRunFrameTimer=0.0f;
                            setStatus("FAULT! Second serve...", 1.8f);
                        }
                    } else {
                        if (lastHitter == 1) {
                            scorePoint(1);
                        } else {
                            scorePoint(2);
                        }
                    }
                    ballInPlay=false;
                    return;
                }

                bouncesOnSide++;
                ballBounced=true;

                if(bouncesOnSide>=2){
                    bool onP1Side = (ballPos.y < pNet);
                    scorePoint(onP1Side ? 2 : 1);
                    ballInPlay=false;
                    return;
                }

                // SMASH TRIGGER
                if(!smashMiniActive
                   && smashCooldownTimer<=0.0f
                   && lastHitter==2
                   && rallyHitCount>=smashTriggerCount
                   && (ballPos.y<pNet)
                   && bouncesOnSide==1
                   && Vector2Distance({ballPos.x,ballPos.y-ballZ},{p1.x,p1.y-PLAYER_DRAW_H*0.45f})<150.0f
                   && !isFaultServeInFlight
                   && !isDoubleFault)
                {
                    setStatus("SMASH OPPORTUNITY!", 2.0f);
                    smashMiniActive=true;
                    smashMiniTimer=SMASH_MINI_DURATION;
                    smashSpacePresses=0;
                    smashReady=true;
                    smashTriggeredThisRally=true;
                }
            }

            // Safety — ball flew way off screen
            if(ballPos.y>WINDOW_HEIGHT+300||ballPos.y<-300||
               ballPos.x<-400||ballPos.x>WINDOW_WIDTH+400){
                if(isFaultServeInFlight){
                    isFaultServeInFlight=false;
                    if(isDoubleFault){
                        isDoubleFault=false;
                        if (lastHitter == 1) {
                            scorePoint(2);
                        } else {
                            scorePoint(1);
                        }
                        setStatus("Double Fault!", 1.2f);
                    } else {
                        serveBarActive=false; serveBarPos=0.0f; serveBarDir=1.0f;
                        p2Move=MOVE_IDLE; cpuRunFrameIdx=0; cpuRunFrameTimer=0.0f;
                        setStatus("FAULT! Second serve...", 1.8f);
                    }
                    ballInPlay=false; return;
                }
                
                if (lastHitter == 1) {
                    scorePoint(2);
                } else {
                    scorePoint(1);
                }
                ballInPlay=false; return;
            }

            // CPU HIT
if(ballPos.y>=pNet && ballVel.y>0 && lastHitter!=2 && ballInPlay){
    if(isFaultServeInFlight||isDoubleFault){
        // no action
    } else {
        bool canVolley=(cpuTactic==CPU_AT_NET||fabs(p2.y-pNet)<130.0f);
        if(lastShotType==SHOT_LOB) canVolley=false;

        bool dropShotDelay=false;
        if(lastShotType==SHOT_DROP){
            if(bouncesOnSide>=1) dropShotDelay=true;
            canVolley=false;
        }

        bool cpuCanHit=(bouncesOnSide>=1)||canVolley;
        if(lastShotType==SHOT_DROP) cpuCanHit=cpuCanHit&&dropShotDelay;

        if(cpuCanHit){
            float visBY=ballPos.y-ballZ;
            float dist=Vector2Distance({ballPos.x,visBY},{p2.x,p2.y-PLAYER_DRAW_H*0.45f});
            float cpuMaxZ=canVolley?120.0f:68.0f;
            // For smashes, use a larger detection radius (180 vs 130)
            float hitDist = (lastShotType == SHOT_SMASH) ? 180.0f : 130.0f;
            if(dist < hitDist && ballZ < cpuMaxZ){
                cpuHitShot();
            }
        }
    }
}
        }
    }

    void draw() override {
        ClearBackground(DARKGREEN);
        drawCourt();
        drawPlayer(p2, p2Move, p2Serve, p2SwingTimer, true, cpuRunFrameIdx);
        drawPlayer(p1, p1Move, p1Serve, p1SwingTimer, false, runFrameIdx);

        if(!ballInPlay){
            if(p1TossVisible && p1TossZ>=0){
                float bx=p1.x+(p1.flipX?-30:30);
                float by=p1.y-PLAYER_DRAW_H*0.7f-p1TossZ*0.15f;
                DrawCircle((int)bx,(int)by,ballRadius,YELLOW);
            }
            if(p2TossVisible && p2TossZ>=0){
                float bx=p2.x+(p2.flipX?-30:30);
                float by=p2.y-PLAYER_DRAW_H*0.7f-p2TossZ*0.15f;
                DrawCircle((int)bx,(int)by,ballRadius,YELLOW);
            }
        }

        drawBall();
        drawScoreboard();
        drawServeBar();
        drawSmashMini();
        drawStatusMsg();

       if (gameOver) {
            string line1;
            Color textColor;
            if (winner == 1) {
                line1 = "Congratulations you win";
                textColor = GOLD;
            } else {
                line1 = "You lost try again";
                textColor = LIGHTGRAY;
            }

            int sz1 = 58;
            int w1 = MeasureText(line1.c_str(), sz1);
            DrawRectangle(0, WINDOW_HEIGHT/2 - 75, WINDOW_WIDTH, 140, Fade(BLACK, 0.85f));
            DrawText(line1.c_str(), (WINDOW_WIDTH - w1) / 2, WINDOW_HEIGHT / 2 - 20, sz1, textColor);
        }
    }
};

TennisGameScene::ResumeData TennisGameScene::resumeData = {};
bool                        TennisGameScene::resumeValid = false;


// ============================================================
// TENNIS RULES SCENE
// ============================================================
class TennisRulesScene : public Scene {
    Texture2D bg;
public:
    TennisRulesScene() { bg = LoadTexture("D:/oop/bkgrnd.png"); }
    ~TennisRulesScene() { UnloadTexture(bg); }
    void update() override {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            changeScene(new TennisGameScene()); // Goes to the Tennis Game
        }
    }
    void draw() override {
        DrawTexturePro(bg, {0,0,(float)bg.width,(float)bg.height},
            {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT}, {0,0}, 0, WHITE);
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.55f));

        const char* title = "TENNIS MATCH RULES";
        int tw = MeasureText(title, 60);
        DrawText(title, (WINDOW_WIDTH-tw)/2, 50, 60, GOLD);

        struct Rule { const char* label; const char* desc; Color col; };
        Rule rules[] = {
            {"MOVEMENT",  "Use W,A,S,D or Arrow Keys to move", WHITE},
            {"HITTING",   "Press SPACE to hit the ball",       SKYBLUE},
            {"LOB SHOT",  "Hold UP + SPACE to hit a Lob",      GREEN},
            {"DROP SHOT", "Hold DOWN + SPACE to hit a Drop",   ORANGE},
            {"DIRECTIONAL SHOTS", " Hold A/D during lob/shot shots for directional hits", PINK},
            {"SMASH",     "Press SPACE x3 during Smash event", YELLOW},
            {"SCORING",   "First to 2 Games wins the match!",  RED},
        };
        float y = 180;
        for (auto& r : rules) {
            int lw = MeasureText(r.label, 26);
            DrawText(r.label, (WINDOW_WIDTH-lw)/2, (int)y, 26, r.col);
            int dw = MeasureText(r.desc, 21);
            DrawText(r.desc, (WINDOW_WIDTH-dw)/2, (int)(y+32), 21, Fade(WHITE, 0.85f));
            y += 84;
        }
        float pulse = 0.6f + 0.4f*sinf(GetTime()*3.0f);
        const char* prompt = "Press SPACE or ENTER to play";
        int pw = MeasureText(prompt, 28);
        DrawText(prompt, (WINDOW_WIDTH-pw)/2, WINDOW_HEIGHT-70, 28, Fade(WHITE, pulse));
    }
};


// ============================================================
// MATCHUP SCENE
// FIX: Now comes AFTER memory game, shows correct player vs cpu assignment
// ============================================================
class MatchupScene : public Scene {
    float timer=0;
    Texture2D bg;
public:
    MatchupScene(){ bg=LoadTexture("D:/oop/bkgrnd.png"); }
    ~MatchupScene(){ UnloadTexture(bg); }
    void update() override {
        timer+=GetFrameTime();
        if(timer>3.5f||IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE))
        changeScene(new TennisRulesScene());
    }
    void draw() override {
        DrawTexturePro(bg,{0,0,(float)bg.width,(float)bg.height},
            {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT},{0,0},0,WHITE);
        DrawRectangle(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,Fade(BLACK,0.6f));
        float alpha=Clamp(timer/0.7f,0.0f,1.0f);

        // FIX: Use g_humanPlayer and g_cpuPlayer directly for correct display
        int hs=52,hw=MeasureText(PLAYER_NAMES[g_humanPlayer],hs);
        DrawText(PLAYER_NAMES[g_humanPlayer],(WINDOW_WIDTH/2-hw)/2,WINDOW_HEIGHT/2-60,hs,Fade(GREEN,alpha));
        string hRank="World Rank #"+to_string(WORLD_RANKS[g_humanPlayer]);
        int hrw=MeasureText(hRank.c_str(),28);
        DrawText(hRank.c_str(),(WINDOW_WIDTH/2-hrw)/2,WINDOW_HEIGHT/2+6,28,Fade(LIGHTGRAY,alpha));

        // YOU label under human player name
        const char* youTag = "(YOU)";
        int ytw = MeasureText(youTag, 22);
        DrawText(youTag, (WINDOW_WIDTH/2-ytw)/2, WINDOW_HEIGHT/2+40, 22, Fade(GREEN, alpha));

        int vsw=MeasureText("VS",72);
        DrawText("VS",(WINDOW_WIDTH-vsw)/2,WINDOW_HEIGHT/2-36,72,Fade(YELLOW,alpha));

        int cs=52,cw=MeasureText(PLAYER_NAMES[g_cpuPlayer],cs);
        DrawText(PLAYER_NAMES[g_cpuPlayer],WINDOW_WIDTH/2+(WINDOW_WIDTH/2-cw)/2,WINDOW_HEIGHT/2-60,cs,Fade(RED,alpha));
        string cRank="World Rank #"+to_string(WORLD_RANKS[g_cpuPlayer]);
        int crw=MeasureText(cRank.c_str(),28);
        DrawText(cRank.c_str(),WINDOW_WIDTH/2+(WINDOW_WIDTH/2-crw)/2,WINDOW_HEIGHT/2+6,28,Fade(LIGHTGRAY,alpha));

        // CPU label under cpu player name
        const char* cpuTag = "(CPU)";
        int ctw = MeasureText(cpuTag, 22);
        DrawText(cpuTag, WINDOW_WIDTH/2+(WINDOW_WIDTH/2-ctw)/2, WINDOW_HEIGHT/2+40, 22, Fade(RED, alpha));

        float pulse=0.5f+0.5f*sinf(GetTime()*3.0f);
        const char* p="Press SPACE to continue";
        int pw=MeasureText(p,26);
        DrawText(p,(WINDOW_WIDTH-pw)/2,WINDOW_HEIGHT-60,26,Fade(WHITE,pulse*alpha));
    }
};


// ============================================================
// MEMORY GAME SCENE
// ============================================================
class MemoryGameScene : public Scene {
    vector<Card> cards;
    Card* first=nullptr, *second=nullptr;
    float flipBackTimer=0;
    bool waiting=false, gameWon=false;
    float winTimer=0, totalTime=0;
    Texture2D backTexture, player0, player1, player2, gameBackground;

    void layoutCards(const vector<Texture2D>& fronts){
        cards.clear();
        for(size_t i=0;i<fronts.size();i++){
            for(int j=0;j<2;j++){
                Card c; c.front=fronts[i]; c.back=backTexture; c.id=(int)i;
                cards.push_back(c);
            }
        }
        mt19937 rng2(time(0));
        shuffle(cards.begin(),cards.end(),rng2);
        float cW=260,cH=360,sX=80,sY=100;
        int perRow=(int)fronts.size();
        float rowW=perRow*cW+(perRow-1)*sX;
        float stX=(WINDOW_WIDTH-rowW)/2;
        float stY=(WINDOW_HEIGHT-(cH+sY))/2 - 80.0f;
        for(size_t i=0;i<cards.size();i++){
            int row=(int)i/perRow, col=(int)i%perRow;
            float rowWi=perRow*cW+(perRow-1)*sX;
            float rowXi=(WINDOW_WIDTH-rowWi)/2;
            cards[i].position={rowXi+col*(cW+sX),stY+row*(cH+sY)};
            cards[i].cardW=cW; cards[i].cardH=cH;
            cards[i].isFlipped=cards[i].isMatched=false;
            cards[i].showFront=false; cards[i].scaleX=1.0f; cards[i].isAnimating=false;
        }
    }
public:
    MemoryGameScene(int dummy=0){
        backTexture   =LoadTexture("D:/oop/card_flip.jpg");
        player0       =LoadTexture("D:/oop/jannik_flip.jpg");
        player1       =LoadTexture("D:/oop/carlos_flip.jpg");
        player2       =LoadTexture("D:/oop/daniil_flip.jpg");
        gameBackground=LoadTexture("D:/oop/bkgrnd_image.jpg");
        vector<Texture2D> fronts={player0,player1,player2};
        layoutCards(fronts);
        first=second=nullptr; waiting=false; gameWon=false; winTimer=0; totalTime=0;
    }
    ~MemoryGameScene(){
        UnloadTexture(backTexture); UnloadTexture(player0);
        UnloadTexture(player1);    UnloadTexture(player2);
        UnloadTexture(gameBackground);
    }
    void update() override {
        // Fix: Freeze the timer exactly when you win!
        if (!gameWon) {
            totalTime += GetFrameTime();
        }
        for(auto& c:cards) c.update();
        if(gameWon){
    winTimer+=GetFrameTime();
    if(winTimer>2.0f){
        // 1. Find the two players you didn't pick
        vector<int> others;
        for(int i=0;i<3;i++) if(i!=g_humanPlayer) others.push_back(i);

        // 2. Identify who is weaker (higher rank number) and stronger
        int weaker = (WORLD_RANKS[others[0]] > WORLD_RANKS[others[1]]) ? others[0] : others[1];
        int stronger = (WORLD_RANKS[others[0]] < WORLD_RANKS[others[1]]) ? others[0] : others[1];

        // 3. Under 10s = Weaker opponent. Over 10s = Stronger opponent.
        if (totalTime <= 10.0f) {
            g_cpuPlayer = weaker;
        } else {
            g_cpuPlayer = stronger;
        }
        
        changeScene(new MatchupScene());
    }
    return;
}
        if(waiting){
            flipBackTimer+=GetFrameTime();
            if(flipBackTimer>1.0f){
                first->flip(); second->flip();
                first=second=nullptr; waiting=false;
            }
            return;
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 mouse=GetMousePosition();
            for(auto& c:cards){
                if(c.isClicked(mouse)&&!c.isFlipped&&!c.isMatched){
                    c.flip();
                    if(!first) first=&c;
                    else if(!second&&&c!=first){
                        second=&c;
                        if(first->id==second->id){ first->isMatched=second->isMatched=true; first=second=nullptr; }
                        else{ waiting=true; flipBackTimer=0; }
                    }
                    break;
                }
            }
        }
        bool allMatched=true;
        for(auto& c:cards) if(!c.isMatched){ allMatched=false; break; }
        if(allMatched&&!gameWon){ gameWon=true; winTimer=0; }
    }
    void draw() override {
        DrawTexturePro(gameBackground,{0,0,(float)gameBackground.width,(float)gameBackground.height},
            {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT},{0,0},0,WHITE);
        DrawRectangle(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,Fade(BLACK,0.4f));
        for(auto& c:cards) c.draw();
        char buf[32]; sprintf(buf,"Time: %.1fs",totalTime);
        DrawText(buf,20,20,28,WHITE);

        const char* hint = "Match all cards! Under 10s = Easy opponent, 10s+ = Hard opponent";
        int hw = MeasureText(hint, 22);
        DrawText(hint, (WINDOW_WIDTH-hw)/2, 24, 22, GOLD);

        if(gameWon){
            const char* msg="YOU WIN!"; int sz=80,w=MeasureText(msg,sz);
            DrawText(msg,(WINDOW_WIDTH-w)/2,WINDOW_HEIGHT/2-40,sz,YELLOW);
        }
    }
};

// ============================================================
// RULES SCENE
// ============================================================
// ============================================================
// MEMORY RULES SCENE
// ============================================================
class MemoryRulesScene : public Scene {
    Texture2D bg;
public:
    MemoryRulesScene() { bg = LoadTexture("D:/oop/bkgrnd.png"); }
    ~MemoryRulesScene() { UnloadTexture(bg); }
    void update() override {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            changeScene(new MemoryGameScene()); // Goes to Memory Game
        }
    }
    void draw() override {
        DrawTexturePro(bg, {0,0,(float)bg.width,(float)bg.height},
            {0,0,(float)WINDOW_WIDTH,(float)WINDOW_HEIGHT}, {0,0}, 0, WHITE);
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.55f));

        const char* title = "MEMORY GAME RULES";
        int tw = MeasureText(title, 60);
        DrawText(title, (WINDOW_WIDTH-tw)/2, 50, 60, GOLD);

        struct Rule { const char* label; const char* desc; Color col; };
        Rule rules[] = {
            {"OBJECTIVE",    "Match all pairs of player cards",                          WHITE},
            {"EASY OPPONENT","Finish under 10 seconds to face the weaker opponent",      YELLOW},
            {"HARD OPPONENT","Finish in 10 seconds or more to face the stronger opponent", RED},
        };
        float y = 200;
        for (auto& r : rules) {
            int lw = MeasureText(r.label, 26);
            DrawText(r.label, (WINDOW_WIDTH-lw)/2, (int)y, 26, r.col);
            int dw = MeasureText(r.desc, 21);
            DrawText(r.desc, (WINDOW_WIDTH-dw)/2, (int)(y+32), 21, Fade(WHITE, 0.85f));
            y += 100;
        }
        float pulse = 0.6f + 0.4f*sinf(GetTime()*3.0f);
        const char* prompt = "Press SPACE or ENTER to start";
        int pw = MeasureText(prompt, 28);
        DrawText(prompt, (WINDOW_WIDTH-pw)/2, WINDOW_HEIGHT-70, 28, Fade(WHITE, pulse));
    }
};




// ============================================================
// LATE IMPLEMENTATIONS
// ============================================================
void PlayerSelectScene::update() {
    timer += GetFrameTime();
    if (fading) {
        fadeOut += GetFrameTime();
        if (fadeOut > 0.6f) {
    g_humanPlayer = selected;
changeScene(new MemoryRulesScene());
}
        return;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) selected = (selected + 2) % 3;
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) selected = (selected + 1) % 3;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) fading = true;

    for (int i = 0; i < 3; i++) {
        float target = (i == selected) ? 1.08f : 1.0f;
        hoverScale[i] += (target - hoverScale[i]) * 0.12f;
    }
}

// ============================================================
// MAIN
// ============================================================
int main(){
    InitWindow(1920,1080,"Tennis Game");
    InitAudioDevice();                      // <-- add this
    SetTargetFPS(60);
    WINDOW_WIDTH=GetScreenWidth(); WINDOW_HEIGHT=GetScreenHeight();
    ToggleFullscreen();
    WINDOW_WIDTH=GetScreenWidth(); WINDOW_HEIGHT=GetScreenHeight();
    menuBackground=LoadTexture("D:/oop/menu_page.png");
    currentScene=new MenuScene();

    while(!WindowShouldClose()){
        WINDOW_WIDTH=GetScreenWidth(); WINDOW_HEIGHT=GetScreenHeight();
        currentScene->update();
        BeginDrawing();
        currentScene->draw();
        EndDrawing();
    }

    if(currentScene) delete currentScene;
    UnloadTexture(menuBackground);
    CloseAudioDevice();                     // <-- add this
    CloseWindow();
    return 0;
}