#### `chapter7`
```cpp
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

...

glutKeyboardFunc(keyboard);
glutSpecialFunc(specialKeys);
```

---

#### `TanningKitty`

```cpp
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    }
    if (state == GLUT_UP) {
        isDragging = false;
    }
}

void motion(int x, int y)
{
    if (isDragging) {
        angleX += (x - lastX);
        angleY += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void mouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
        zoomZ -= 0.2f; // zoom in
    else
        zoomZ += 0.2f; // zoom out

    if (zoomZ < 1.5f) zoomZ = 1.5f;
    if (zoomZ > 20.0f) zoomZ = 20.0f;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':  // �� ��
    case 'A':
        zoomZ -= 0.2f;
        if (zoomZ < 1.5f) zoomZ = 1.5f;
        glutPostRedisplay();
        break;
    case 'z':  // �� �ƿ�
    case 'Z':
        zoomZ += 0.2f;
        if (zoomZ > 20.0f) zoomZ = 20.0f;
        glutPostRedisplay();
        break;
    }
}

...

glutMouseFunc(mouse);
glutMotionFunc(motion);
glutKeyboardFunc(keyboard);  // Ű���� �Է� ���
```

---

#### `LEGOSpiderman`

```cpp
void myKeyboard(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key) {
	case 'a':
		scale *= scaleDelta;
		break;
	case 'z':
		scale /= scaleDelta;
		break;
	}
	glutPostRedisplay();
}

void myMouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (aniOn) glutIdleFunc(nullptr);
		mouseState = state;
		mouseButton = btn;
		mouseX = x;
		mouseY = y;
	} else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouseState = -1;
		if (aniOn) glutIdleFunc(myIdle);
	} else {
		return;
	}
	glutPostRedisplay();
}

void myMotion(int x, int y)
{
	if (mouseButton == GLUT_LEFT_BUTTON && mouseState == GLUT_DOWN) {
		rot[1] -= static_cast<float>(mouseX - x) / 2.f;
		rot[0] -= static_cast<float>(mouseY - y) / 2.f;
		glutPostRedisplay();
		mouseX = x;
		mouseY = y;
	}
}

...

glutKeyboardFunc(myKeyboard);
glutMouseFunc(myMouse);
glutMotionFunc(myMotion);
```

---

#### `IULastFantasy`

```cpp
// ZOOM
void keyboard( unsigned char key, int x, int y )
{
    (void)x; (void)y;
    switch(key)
    {
        case 'a':
            zoom--;
            break;
        case 'z':
            zoom++;
            break;
    }
}

// VIEWPOINT 1
void processMouse(int button, int state, int x, int y)
{

    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            beforeX = x;
            beforeY = y;
        }
    }
}

// VIEWPOINT 2
void processMouseMotion(int x, int y)
{
    if(abs(beforeX-x) > abs(beforeY-y)){
        if(beforeX < x)
        {
            theta -= 0.1f;
        }else if(beforeX > x){
            theta += 0.1f;
        }
    }else {
        if(beforeY > y){
            phi -= 0.1f;
        }else if(beforeY < y){
            phi -= 0.1f;
        }
    }

    beforeX = x;
    beforeY = y;

    glutPostRedisplay();

    if ( theta > 2.0f * PI )
        theta -= (2.0f * PI);
    else if ( theta < 0.0f )
        theta += (2.0f * PI);
}

...

glutMouseFunc(processMouse);
glutMotionFunc(processMouseMotion);
glutKeyboardFunc(keyboard);
```

---

#### `Doraemon`

```cpp

// ====== 입력 처리 ======
void myKeyboard(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	switch (key) {
	case 'z':
		viewport += 5;
		glutPostRedisplay();
		break;
	case 'a':
		viewport -= 5;
		glutPostRedisplay();
		break;
	case 'q':
		std::exit(0);
		break;
	}
}

void myMouseMotion(GLint x, GLint y)
{
	if (x - moveX > 0)
		gYAngle += 5.0f;
	else if (x - moveX < 0)
		gYAngle -= 5.0f;

	if (y - moveY > 0)
		gXAngle += 5.0f;
	else if (y - moveY < 0)
		gXAngle -= 5.0f;

	moveX = x;
	moveY = y;
	glutPostRedisplay();
}

...

glutKeyboardFunc(myKeyboard);
glutMotionFunc(myMouseMotion);
```

---

#### `CharlieBrown`

```cpp
void keyboard(unsigned char key, int, int) {
    if (key == 'a') zoom *= 0.9f;
    if (key == 'z') zoom *= 1.1f;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = false;
    }
#ifdef __APPLE__
    // macOS GLUT: 마우스 휠이 button 3(up)/4(down)으로 전달
    if (state == GLUT_DOWN) {
        if (button == 3) { zoom -= 0.4f; glutPostRedisplay(); }
        if (button == 4) { zoom += 0.4f; glutPostRedisplay(); }
        if (zoom < 1.0f) zoom = 1.0f;
        if (zoom > 5.0f) zoom = 5.0f;
    }
#endif
}

void motion(int x, int y) {
    if (isDragging) {
        rotateX += (y - lastY);
        rotateY += (x - lastX);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

...

glutKeyboardFunc(keyboard);
glutMouseFunc(mouse);
glutMotionFunc(motion);
```

> ### Keyboard

```cpp
void keyboard(unsigned char key, int, int) {
    if (key == 'a') zoom *= 0.9f;
    if (key == 'z') zoom *= 1.1f;
    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	switch (key) {
	case 'z':
		viewport += 5;
		glutPostRedisplay();
		break;
	case 'a':
		viewport -= 5;
		glutPostRedisplay();
		break;
	case 'q':
		std::exit(0);
		break;
	}
}

void keyboard(unsigned char key, int x, int y )
{
    (void)x; (void)y;
    switch(key)
    {
        case 'a':
            zoom--;
            break;
        case 'z':
            zoom++;
            break;
    }
}

void myKeyboard(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key) {
	case 'a':
		scale *= scaleDelta;
		break;
	case 'z':
		scale /= scaleDelta;
		break;
	}
	glutPostRedisplay();
}

// ZOOM
void keyboard( unsigned char key, int x, int y )
{
    (void)x; (void)y;
    switch(key)
    {
        case 'a':
            zoom--;
            break;
        case 'z':
            zoom++;
            break;
    }
}

void myKeyboard(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key) {
	case 'a':
		scale *= scaleDelta;
		break;
	case 'z':
		scale /= scaleDelta;
		break;
	}
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
```