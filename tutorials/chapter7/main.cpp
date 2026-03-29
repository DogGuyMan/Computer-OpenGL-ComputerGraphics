/*
 * Toon Shading Demo - Fixed-Function Pipeline Only
 * freeglut + GL 2.1 Compatibility Profile
 * 
 * Techniques:
 *   1) 1D Ramp Texture via glTexGen (N·L -> discrete color bands)
 *   2) Back-face outline (scaled + front-face culled black pass)
 *
 * Controls:
 *   Left/Right arrow: rotate light
 *   Up/Down arrow:    change outline thickness
 *   1/2/3:            switch ramp style (2-tone, 3-tone, smooth-step)
 *   ESC:              quit
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <string>

/* ── State ── */
static float g_lightAngle  = 45.0f;
static float g_outlineScale = 1.04f;
static int   g_rampStyle    = 0;        /* 0=2-tone, 1=3-tone, 2=smooth-step */
static float g_rotY         = 0.0f;     /* auto-rotate sphere */

static GLuint g_rampTex = 0;

/* ── 1-D Ramp Texture Generation ── */
#define RAMP_SIZE 256

static void buildRamp(unsigned char *pixels, int style)
{
    int i;
    for (i = 0; i < RAMP_SIZE; i++) {
        float t = (float)i / (float)(RAMP_SIZE - 1);  /* 0..1 */
        unsigned char v;

        switch (style) {
        case 0: /* 2-tone: hard shadow/lit split */
            v = (t < 0.45f) ? 80 : 230;
            break;

        case 1: /* 3-tone: shadow / mid / highlight */
            if      (t < 0.30f) v = 60;
            else if (t < 0.60f) v = 155;
            else                v = 235;
            break;

        case 2: /* smooth-step bands (softer cartoon feel) */
        {
            /* 3 smooth-step transitions */
            float s = 0.0f;
            /* band 1 */
            { float e0=0.15f,e1=0.35f,x=(t-e0)/(e1-e0);
              if(x<0)x=0; if(x>1)x=1; s+=x*x*(3-2*x)*0.35f; }
            /* band 2 */
            { float e0=0.40f,e1=0.60f,x=(t-e0)/(e1-e0);
              if(x<0)x=0; if(x>1)x=1; s+=x*x*(3-2*x)*0.35f; }
            /* band 3 */
            { float e0=0.65f,e1=0.80f,x=(t-e0)/(e1-e0);
              if(x<0)x=0; if(x>1)x=1; s+=x*x*(3-2*x)*0.30f; }
            v = (unsigned char)(s * 255.0f);
            break;
        }
        default:
            v = (unsigned char)(t * 255.0f);
            break;
        }
        /* RGB = uniform gray-ish tint; we modulate with material color later */
        pixels[i * 3 + 0] = v;
        pixels[i * 3 + 1] = v;
        pixels[i * 3 + 2] = v;
    }
}

static void uploadRamp(int style)
{
    unsigned char pixels[RAMP_SIZE * 3];
    buildRamp(pixels, style);

    if (!g_rampTex)
        glGenTextures(1, &g_rampTex);

    glBindTexture(GL_TEXTURE_1D, g_rampTex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, RAMP_SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    /* NEAREST = hard band edges; LINEAR would blur them */
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

/* ── Texture-Coordinate Generation Setup ──
 *
 * The trick: use GL_OBJECT_LINEAR texgen with the plane equation set
 * to the light direction.  OpenGL computes  s = dot(vertex, plane)
 * per-vertex.  Since we're lighting a unit sphere centered at origin,
 * the vertex position IS the normal, so  s ≈ N·L.
 *
 * We shift the result into [0,1] via the texture matrix.
 */
static void setupTexGen(void)
{
    float rad = g_lightAngle * 3.14159265f / 180.0f;
    /* light direction (normalized, object space) */
    float lx = cosf(rad);
    float ly = 0.4f;          /* slight vertical component */
    float lz = sinf(rad);
    float len = sqrtf(lx*lx + ly*ly + lz*lz);
    lx /= len;  ly /= len;  lz /= len;

    /* texgen plane = light direction */
    GLfloat plane[4] = { lx, ly, lz, 0.0f };

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane);
    glEnable(GL_TEXTURE_GEN_S);

    /* dot product gives [-1,1]; map to [0,1] via texture matrix:
       s' = s * 0.5 + 0.5  */
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f, 0.0f, 0.0f);
    glScalef(0.5f, 1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* ── Drawing ── */

static void drawSphere(void)
{
    glutSolidSphere(1.0, 64, 64);
}

static void drawOutline(void)
{
    /* Pass: render back-faces only, scaled up, in solid black */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);          /* cull front → draw back faces */

    glDisable(GL_TEXTURE_1D);
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);

    glPushMatrix();
    glScalef(g_outlineScale, g_outlineScale, g_outlineScale);
    glutSolidSphere(1.0, 64, 64);
    glPopMatrix();

    glCullFace(GL_BACK);           /* restore */
}

static void drawToonSphere(void)
{
    /* 1) Outline pass */
    drawOutline();

    /* 2) Toon-shaded pass */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, g_rampTex);
    setupTexGen();

    /* Base color tint via texture environment:
       MODULATE means  finalColor = texture * primaryColor.
       We set a warm-ish primary color so the ramp tints it. */
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glDisable(GL_LIGHTING);        /* we're doing our own "lighting" via ramp */
    glColor3f(0.85f, 0.35f, 0.25f);  /* base material: warm red-orange */

    drawSphere();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_1D);
}

/* ── HUD text ── */
static void drawText(float x, float y, const char *str)
{
    const char *p;
    glRasterPos2f(x, y);
    for (p = str; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}

static void drawHUD(void)
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    const char *names[] = { "2-tone", "3-tone", "smooth-step" };
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glColor3f(1, 1, 1);
    
    
    std::string rampStr = "Ramp: " + std::string(names[g_rampStyle]) + " [1/2/3]";
    drawText(10, h - 20, rampStr.c_str());
    std::string outlineStr = "Outline: " + std::to_string(g_outlineScale).substr(0, std::to_string(g_outlineScale).find('.') + 3) + " [Up/Down]";
    drawText(10, h - 36, outlineStr.c_str());
    std::string lightStr = "Light angle: " + std::to_string(static_cast<int>(g_lightAngle)) + " [Left/Right]";
    drawText(10, h - 52, lightStr.c_str());

    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/* ── GLUT Callbacks ── */

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0.5, 3.5,  0, 0, 0,  0, 1, 0);

    glRotatef(g_rotY, 0, 1, 0);
    drawToonSphere();

    drawHUD();
    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void idle(void)
{
    g_rotY += 0.3f;
    if (g_rotY > 360.0f) g_rotY -= 360.0f;
    glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    switch (key) {
    case 27: exit(0); break;
    case '1': g_rampStyle = 0; uploadRamp(0); break;
    case '2': g_rampStyle = 1; uploadRamp(1); break;
    case '3': g_rampStyle = 2; uploadRamp(2); break;
    }
    glutPostRedisplay();
}

static void specialKeys(int key, int x, int y)
{
    (void)x; (void)y;
    switch (key) {
    case GLUT_KEY_LEFT:  g_lightAngle -= 5.0f; break;
    case GLUT_KEY_RIGHT: g_lightAngle += 5.0f; break;
    case GLUT_KEY_UP:    g_outlineScale += 0.005f; break;
    case GLUT_KEY_DOWN:
        g_outlineScale -= 0.005f;
        if (g_outlineScale < 1.0f) g_outlineScale = 1.0f;
        break;
    }
    glutPostRedisplay();
}

/* ── Main ── */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Toon Shading - Fixed Function Pipeline");

    /* GL state */
    glClearColor(0.15f, 0.15f, 0.20f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /* Build initial ramp */
    uploadRamp(g_rampStyle);

    printf("=== Toon Shading Demo (No Shaders!) ===\n");
    printf("  1/2/3      : ramp style\n");
    printf("  Left/Right : rotate light\n");
    printf("  Up/Down    : outline thickness\n");
    printf("  ESC        : quit\n");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}