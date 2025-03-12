#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// display constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// player constants
const float PLAYER_WIDTH = 10.0f;
const float PLAYER_HEIGHT = 80.0f;
const float PLAYER_OFFSET = 20.0f;

// ball
const float ball_radius = 8.0f;
float ball_x = WINDOW_WIDTH / 2;
float ball_y = WINDOW_HEIGHT / 2;
float ball_speed_x = 6.0f;
float ball_speed_y = 6.0f;
float ball_acceleration = 0.2f;

// players
const float player_speed = 5.0f;
float player1_y = WINDOW_HEIGHT / 2;
float player2_y = WINDOW_HEIGHT / 2;

// input state
bool key_states[256] = {false};
bool special_states[256] = {false};

// scores
int player1_score = 0;
int player2_score = 0;

bool game_started = false;

void initialize(void);
void display();
void draw_ball(float pos_x, float pos_y, float radius, int segments);
void draw_player(float pos_x, float pos_y);
void draw_score();
void update_game();
void update_ball();
void reset_ball();
void start_game(int value);
void key_pressed(unsigned char key, int x, int y);
void key_released(unsigned char key, int x, int y);
void special_pressed(int key, int x, int y);
void special_released(int key, int x, int y);
void timer(int step);
void reshape(int w, int h);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pong");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_pressed);
	glutKeyboardUpFunc(key_released);
	glutSpecialFunc(special_pressed);
	glutSpecialUpFunc(special_released);
	glutTimerFunc(16, timer, 0);
	glutMainLoop();
	return 0;
}

void initialize(void)
{
	srand(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);        // don't interpolate colors (efficiency)
	glutTimerFunc(1000, start_game, 0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw_ball(ball_x, ball_y, ball_radius, 100);
	draw_player(PLAYER_OFFSET, player1_y);
	draw_player(WINDOW_WIDTH - PLAYER_OFFSET - PLAYER_WIDTH, player2_y);
	draw_score();
	glFlush();
}

void draw_ball(float pos_x, float pos_y, float radius, int segments)
{
	// we move the coordinate system so that (0, 0) is the ball's center
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0.0f);

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	float angle;
	for (int i = 0; i <= segments; ++i) {
		angle = i * 2.0f * M_PI / segments; // M_PI from math.h
		glVertex2f(radius * cos(angle), radius * sin(angle));
	}
	glEnd();
	glPopMatrix();		// clear matrix transforms
}

void draw_player(float pos_x, float pos_y)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(pos_x, pos_y + PLAYER_HEIGHT / 2);
	glVertex2f(pos_x + PLAYER_WIDTH, pos_y + PLAYER_HEIGHT / 2);
	glVertex2f(pos_x + PLAYER_WIDTH, pos_y - PLAYER_HEIGHT / 2);
	glVertex2f(pos_x, pos_y - PLAYER_HEIGHT / 2);
	glEnd();
}

void draw_score() {
	char score_text[20];
	sprintf(score_text, "%d : %d", player1_score, player2_score);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(WINDOW_WIDTH/2 - 20, WINDOW_HEIGHT - 20);
	for (char* c = score_text; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void update_game() {
	// exit
	if (key_states[27]) {
		exit(0);
	}

	// player 1
	if (key_states['w'] || key_states['W']) {
		if (player1_y < WINDOW_HEIGHT - (PLAYER_HEIGHT / 2))
			player1_y += player_speed;
	}
	if (key_states['s'] || key_states['S']) {
		if (player1_y > (PLAYER_HEIGHT / 2))
			player1_y -= player_speed;
	}

	// player 2
	if (special_states[GLUT_KEY_UP]) {
		if (player2_y < WINDOW_HEIGHT - (PLAYER_HEIGHT / 2))
			player2_y += player_speed;
	}
	if (special_states[GLUT_KEY_DOWN]) {
		if (player2_y > (PLAYER_HEIGHT / 2))
			player2_y -= player_speed;
	}

	update_ball();
	glutPostRedisplay();
}

void update_ball()
{
	if (!game_started) return;

	ball_x += ball_speed_x;
	ball_y += ball_speed_y;

	// floor and ceiling collisions
	if (ball_y + ball_radius > WINDOW_HEIGHT) {
		ball_y = WINDOW_HEIGHT - ball_radius;
		ball_speed_y = -ball_speed_y;
	} else if (ball_y - ball_radius < 0) {
		ball_y = ball_radius;
		ball_speed_y = -ball_speed_y;
	}

	// left player collision
	if (ball_x - ball_radius < PLAYER_OFFSET + PLAYER_WIDTH
            && ball_x - ball_radius > PLAYER_OFFSET
            && ball_y > player1_y - PLAYER_HEIGHT/2
            && ball_y < player1_y + PLAYER_HEIGHT/2) {

		ball_x = PLAYER_OFFSET + PLAYER_WIDTH + ball_radius;
		ball_speed_x = -ball_speed_x + ball_acceleration;

		// change vertical speed depending on where the ball hits
		float intersect_y = (player1_y - ball_y) / (PLAYER_HEIGHT / 2);
		ball_speed_y = intersect_y * -5.0f;
	}

	// right player collision
	if (ball_x + ball_radius > WINDOW_WIDTH - PLAYER_OFFSET - PLAYER_WIDTH
            && ball_x + ball_radius < WINDOW_WIDTH - PLAYER_OFFSET
            && ball_y > player2_y - PLAYER_HEIGHT/2
            && ball_y < player2_y + PLAYER_HEIGHT/2) {

		ball_x = WINDOW_WIDTH - PLAYER_OFFSET - PLAYER_WIDTH - ball_radius;
		ball_speed_x = -ball_speed_x - ball_acceleration;;

		// change vertical speed depending on where the ball hits
		float intersect_y = (player2_y - ball_y) / (PLAYER_HEIGHT/2);
		ball_speed_y = intersect_y * -5.0f;
	}

	// out of bounds
	if (ball_x + ball_radius > WINDOW_WIDTH) {
		player1_score++;
		reset_ball();
	} else if (ball_x - ball_radius < 0) {
		player2_score++;
		reset_ball();
	}
}

void reset_ball()
{
	ball_x = WINDOW_WIDTH / 2;
	ball_y = WINDOW_HEIGHT / 2;
	// move towards point winner
	ball_speed_x = -ball_speed_x;
	printf("%f\n", ball_speed_x);
	// random y direction
	ball_speed_y = (rand() % 2 - 1) * ball_speed_y;
	game_started = false;
	glutTimerFunc(1000, start_game, 0); // start after waiting 1 second
}


// used in a delayed callback
void start_game(int value)
{
	game_started = true;
}

// normal keys
void key_pressed(unsigned char key, int x, int y)
{
	key_states[key] = true;
	glutPostRedisplay();
}

void key_released(unsigned char key, int x, int y)
{
	key_states[key] = false;
}

// special keys (function keys, arrow keys, space, etc.)
void special_pressed(int key, int x, int y) {
	special_states[key] = true;
	glutPostRedisplay();
}

void special_released(int key, int x, int y) {
	special_states[key] = false;
}

// consistent timestep of 16ms ~ 60fps
void timer(int value) {
	update_game();
	glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h)
{
	float aspect = (float)w / (float)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// we keep the 4:3 aspect ratio by adding (pillar|letter)boxes
	if (aspect >= ASPECT_RATIO) {
		float adjusted_width = h * ASPECT_RATIO;
		float margin = (w - adjusted_width) / 2;
		glViewport(margin, 0, adjusted_width, h);
	} else {
		float adjusted_height = w / ASPECT_RATIO;
		float margin = (h - adjusted_height) / 2;
		glViewport(0, margin, w, adjusted_height);
	}
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
