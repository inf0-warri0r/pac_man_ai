/*
*Author :Tharindra Galahena
*Project:pac-man A.I.
*Date   :02/10/2012
*License:
* 
*     Copyright 2012 Tharindra Galahena
*
* This program is free software: you can redistribute it and/or modifood_y it under the terms of 
* the GNU General Public License as published by the Free Software Foundation, either 
* version 3 of the License, or (at your option) any later version. This program is distributed
* in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with This program. 
* If not, see http://www.gnu.org/licenses/.
*
*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <fstream>

#define MAX 20

typedef struct node{
	int x;
	int y;
	struct node *p;
} node;

typedef struct onode{
	int f;
	node *n;
	struct onode *next;
} onode;

typedef struct cnode{
	node *n;
	struct cnode *next;
} cnode;

typedef struct pill{
	int x;
	int y;
	struct pill *next;
} pill;

typedef struct ghost{
	int x;
	int y;
	bool sleep;
	bool caught;
} ghost;

int tx = 1;
int ty = 1;

int txg1 = 5;
int txg2 = 10;
int tyg1 = 5;
int tyg2 = 10;

int counter = 0;
int sleep_count = 0;
int score = 0;
int life = 3;

int px = 9;
int py = 12;

char grid [MAX][MAX];
bool f = false;
int tpm = 200;

bool is_sleep = false;

ghost *g1, *g2;
pill  *pillist = NULL;

using namespace std;

void add_pillist(int x, int y){
	pill *tmp = (pill *)malloc(sizeof(pill));
	tmp -> x = x;
	tmp -> y = y;
	tmp -> next = pillist;
	pillist = tmp;
}
void del_pillist(int x, int y){
	if(pillist == NULL) return ;
	if(pillist -> x == x && pillist -> y == y){
		pillist = pillist -> next;
		return;
	}
	pill *p = pillist;
	while(p -> next != NULL){
		if(p -> next -> x == x && p -> next -> y == y){
			p -> next = p -> next -> next;
			return;
		}
		p = p -> next;
	}
	
}
pill* closest_pill(int x, int y){
	int tx1 = g1 -> x;
	int ty1 = g1 -> y;
	int tx2 = g2 -> x;
	int ty2 = g2 -> y;
	
	pill *p = pillist;
	int min = 1000000000;
	pill* min_pill = pillist;
	while(p != NULL){
		int l = (p -> x - x) * (p -> x - x) +
				(p -> y - y) * (p -> y - y);
		if(l < min){
			if(x < tx1 - 1 || x > tx1 + 1 || y < ty1 - 1 || y > ty1 + 1){
				if(x < tx2 - 1 || x > tx2 + 1 || y < ty2 - 1 || y > ty2 + 1){
					min = l;
					min_pill = p;
				}
			}
		}
		p = p -> next;
	}
	return min_pill;
}
cnode* add_close_list(node *n, cnode *clist){
	cnode *tmp = (cnode *)malloc(sizeof(cnode));
	tmp -> n = n;
	tmp -> next = clist;
	clist = tmp;
	return clist;
}

ghost *nearest_ghost(){
	int l1 = (g1 -> x) * (g1 -> x) + (g1 -> y) * (g1 -> y);
	int l2 = (g2 -> x) * (g2 -> x) + (g2 -> y) * (g2 -> y);
	if(l1 < l2) return g1;
	else return g2;
}

void new_t(){
	if(!is_sleep){
		pill* p = closest_pill(px, py);
		tx = p -> x;
		ty = p -> y;
	}else{
		if(!g1 -> caught && !g2 -> caught){ 
			ghost *g = nearest_ghost();
			tx = g -> x;
			ty = g -> y;
		}else if(!g1 -> caught){
			tx = g1 -> x;
			ty = g1 -> y;
		}else if(!g2 -> caught){
			tx = g2 -> x;
			ty = g2 -> y;
		}else{
			pill* p = closest_pill(px, py);
			tx = p -> x;
			ty = p -> y;
		}
	}
}
void new_t_g1(){
	while(1){
		txg1 = rand() % 20;
		tyg1 = rand() % 20;
		if(grid[tyg1][txg1] != '0'){
			return;
		}
	}
}
void new_t_g2(){
	while(1){
		txg2 = rand() % 20;
		tyg2 = rand() % 20;
		if(grid[tyg2][txg2] != '0'){
			return;
		}
	}
}
onode* add_open_list(node *n, int f, onode *list){
	onode *tmp = (onode *)malloc(sizeof(onode));
	tmp -> n = n;
	tmp -> f = f;
	if(list == NULL){
		list = tmp;
		tmp -> next = NULL;
		return list;
	}else if(f < list -> f){
		tmp -> next = list;
		list = tmp;
		return list;
	}else{
		onode *p = list;
		while(p -> next != NULL){
			if(f < p -> next -> f){
				tmp -> next = p -> next;
				p -> next = tmp;
				return list;
			}
			p = p -> next;
		}
		tmp -> next = p -> next;
		p -> next = tmp;
	}
	return list;
}
bool check1(int x, int y, cnode *close_list){
	cnode *p = close_list;
	while(p != NULL){
		if(p -> n -> x == x && p -> n -> y == y) return true;
		p = p -> next;
	}
	if(px == g1 -> x && py == g1 -> y) return true;
	if(px == g2 -> x && py == g2 -> y) return true;
	return false;
}
bool check(int x, int y, cnode *close_list){
	cnode *p = close_list;
	while(p != NULL){
		if(p -> n -> x == x && p -> n -> y == y) return true;
		p = p -> next;
	}
	return false;
}
onode * del_open_list(onode *n, onode *openlist){
	onode *p = openlist;
	if(p == NULL) return openlist;
	if(p == n){ 
		openlist = p -> next;
		return openlist;
	}
	while(p -> next != NULL){
		if(p -> next == n) p -> next = p -> next -> next;
		p = p -> next;
	} 
	return openlist;
}

int get_f(int x, int y){
		int dx;
		int dy;
		dx = tx - x;
		dy = ty - y;
		if(dx < 0) dx = -dx;
		if(dy < 0) dy = -dy;
		
		int dx1;
		int dy1;
		int dx2;
		int dy2;
				
		dx1 = g1 -> x - x;
		dy1 = g1 -> y - y;
		dx2 = g2 -> x - x;
		dy2 = g2 -> y - y;
			
		if(dx1 < 0) dx1 = -dx1;
		if(dy1 < 0) dy1 = -dy1;
		if(dx2 < 0) dx2 = -dx2;
		if(dy2 < 0) dy2 = -dy2;
		int g = 0;
		if(grid[y][x] == '2'){
			g = 0;
		}else{
			g = 500;
		}
		int g1 = (40 - dx1 - dy1)*(40 - dx1 - dy1)*(40 - dx1 - dy1)*800;
		if(dx1 > 10 && dy1 > 10) g1 = 0;
		int g2 = (40 - dx2 - dy2)*(40 - dx2 - dy2)*(40 - dx2 - dy2)*800;
		if(dx2 > 10 && dy2 > 10) g2 = 0;
		int f;
		if(!is_sleep)
			f = g + g1 + g2 + (dx + dy) * 30;
		else
			f = g + (dx + dy) * 30;
		return f;
}

int get_f_g(int x, int y, int tx, int ty){
		
		int dx;
		int dy;
		dx = tx - x;
		dy = ty - y;
		if(dx < 0) dx = -dx;
		if(dy < 0) dy = -dy;
		
		int g = 10;
		int f = g + (dx + dy) * 10;
		return f;
}
onode* set_open_list(node *n, onode *openlist, cnode *close_list){
	int x = n -> x;
	int y = n -> y;
	int i = 0;
	if(grid[y + 1][x] != '0' && !check1(x, y + 1, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x;
		tmp -> y = y + 1;
		tmp -> p = n;
		int f = get_f(tmp -> x, tmp -> y);
		
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y - 1][x] != '0' && !check1(x, y - 1, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x;
		tmp -> y = y - 1;
		tmp -> p = n;
		int f = get_f(tmp -> x, tmp -> y);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y][x + 1] != '0' && !check1(x + 1, y, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x + 1;
		tmp -> y = y;
		tmp -> p = n;
		int f = get_f(tmp -> x, tmp -> y);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y][x - 1] != '0' && !check1(x - 1, y, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x - 1;
		tmp -> y = y;
		tmp -> p = n;
		
		int f = get_f(tmp -> x, tmp -> y);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	return openlist;
}
onode * set_open_list_g(node *n, onode *openlist, cnode *close_list, int tx, int ty){
	int x = n -> x;
	int y = n -> y;
	int i = 0;
	if(grid[y + 1][x] != '0' && !check(x, y + 1, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x;
		tmp -> y = y + 1;
		tmp -> p = n;
		int f = get_f_g(tmp -> x, tmp -> y, tx, ty);
		
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y - 1][x] != '0' && !check(x, y - 1, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x;
		tmp -> y = y - 1;
		tmp -> p = n;
		int f = get_f_g(tmp -> x, tmp -> y, tx, ty);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y][x + 1] != '0' && !check(x + 1, y, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x + 1;
		tmp -> y = y;
		tmp -> p = n;
		int f = get_f_g(tmp -> x, tmp -> y, tx, ty);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	if(grid[y][x - 1] != '0' && !check(x - 1, y, close_list)){
		node *tmp = (node *) malloc(sizeof(node));
		tmp -> x = x - 1;
		tmp -> y = y;
		tmp -> p = n;
		
		int f = get_f_g(tmp -> x, tmp -> y, tx, ty);
		openlist = add_open_list(tmp, f, openlist);
		i++;
	}
	return openlist;
}
onode *next_move(onode *openlist){
	int min = 999999999;
	onode *min_node;
	int x, y;
	onode *p = openlist;

	while(p != NULL){
		int f = p -> f;
		
		if(f < min){
			min = f;
			min_node = p;
		}
		p = p -> next;
	}
	if(min == 999999999) return NULL;
	return min_node;
}
void par(float x1, float x2, float y1, float y2, float r, float b, float g){
	glColor3f(r, b, g);
	glBegin(GL_QUADS);
	
	glVertex3f(x1, y1, 0.0);
	glVertex3f(x2, y1, 0.0);
	glVertex3f(x2, y2, 0.0);
	glVertex3f(x1, y2, 0.0);

	glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glTranslatef(0.0, 0.0, -22.0);
	par(-6.0 + 0.6 * px + 0.1,
		-6.0 + 0.6 * (px + 1) - 0.1,
		6.0 - 0.6 * py + 0.1,
		6.0 - 0.6 * (py - 1) - 0.1, 1.0, 1.0, 0.0);
	if(g1 -> sleep)
		par(-6.0 + 0.6 * g1 -> x + 0.1,
			-6.0 + 0.6 * (g1 -> x + 1) - 0.1,
			6.0 - 0.6 * g1 -> y + 0.1,
			6.0 - 0.6 * (g1 -> y - 1) - 0.1, 0.0, 0.0, 1.0);
	else
		par(-6.0 + 0.6 * g1 -> x + 0.1,
			-6.0 + 0.6 * (g1 -> x + 1) - 0.1,
			6.0 - 0.6 * g1 -> y + 0.1,
			6.0 - 0.6 * (g1 -> y - 1) - 0.1, 1.0, 0.0, 0.0);
			
	if(g2 -> sleep)		
		par(-6.0 + 0.6 * g2 -> x + 0.1,
			-6.0 + 0.6 * (g2 -> x + 1) - 0.1,
			6.0 - 0.6 * g2 -> y + 0.1,
			6.0 - 0.6 * (g2 -> y - 1) - 0.1, 0.0, 0.0, 1.0);
	else
		par(-6.0 + 0.6 * g2 -> x + 0.1,
			-6.0 + 0.6 * (g2 -> x + 1) - 0.1,
			6.0 - 0.6 * g2 -> y + 0.1,
			6.0 - 0.6 * (g2 -> y- 1) - 0.1, 0.0, 1.0, 1.0);

	for(int i = 0; i < MAX; i++){
		for(int j = 0; j < MAX; j++){
			if(grid[i][j] == '0')
				par(-6.0 + 0.6 * j + 0.0,
					-6.0 + 0.6 * (j + 1),
					6.0 - 0.6 * i + 0.0,
					6.0 - 0.6 * (i - 1), 0.0, 0.0, 1.0);
			else if(grid[i][j] == '2')
				par(-6.0 + 0.6 * j + 0.2,
					-6.0 + 0.6 * (j + 1) - 0.2,
					6.0 - 0.6 * i + 0.2,
					6.0 - 0.6 * (i - 1) - 0.2, 1.0, 1.0, 1.0);
		}
	}
	
	glutSwapBuffers();
}
node* get_path(node *n, node *path){

	node *p = n;
	int i = 0;
	while(p != NULL){
		node *tmp = (node *)malloc(sizeof(node));
		tmp -> x = p -> x;
		tmp -> y = p -> y;
		if(path == NULL) path = tmp;
		else{
			tmp -> p = path;
			path = tmp;
		}
		p = p -> p;
	}
	return path;
}
node *find_path(node *current_node){
	onode *openlist = NULL;
	cnode *close_list = NULL;
	node *path = NULL;
	int px, py;
	while(1){
		close_list = add_close_list(current_node, close_list);	
		openlist = set_open_list(current_node, openlist, close_list);
		onode *min = next_move(openlist);
		node *p;
		if(min == NULL){
			path = get_path(current_node, path);
			return path;	
		}else{
			openlist = del_open_list(min, openlist);
			p = min -> n;
			current_node = p;
			px = current_node -> x;
			py = current_node -> y;
		}
		if(px == tx && py == ty){
			path = get_path(current_node, path);
			return path;
		}
	}
	return path;
}
node* find_path2(node *current_node, int tx, int ty){
	onode *openlist = NULL;
	cnode *close_list = NULL;
	node *path = NULL;
	int px, py;
	while(1){
		close_list = add_close_list(current_node, close_list);	
		openlist = set_open_list_g(current_node, openlist, close_list, tx, ty);
		onode *min = next_move(openlist);
		node *p;
		if(min == NULL){
			path = get_path(current_node, path);
			return path;
		}else{
			openlist = del_open_list(min, openlist);
			p = min -> n;
			current_node = p;
			px = current_node -> x;
			py = current_node -> y;
		}
		if(px == tx && py == ty){
			path = get_path(current_node, path);
			return path;
		}
	}
	return path;
}
void myIdleFunc(int a) {
	
	if(life <= 0){
		cout << "game lost !!! "<< endl;
		exit(0);
	}
	
	node *path = NULL;
	node *path_g1 = NULL;
	node *path_g2 = NULL;
	
	node *current_node = (node *)malloc(sizeof(node));
	current_node -> x = px;
	current_node -> y = py;
	current_node -> p = NULL;
	
	node *current_node_g1 = (node *)malloc(sizeof(node));
	current_node_g1 -> x = g1 -> x;
	current_node_g1 -> y = g1 -> y;
	current_node_g1 -> p = NULL;
	
	node *current_node_g2 = (node *)malloc(sizeof(node));
	current_node_g2 -> x = g2 -> x;
	current_node_g2 -> y = g2 -> y;
	current_node_g2 -> p = NULL;
	
	counter++;
	sleep_count++;
	if(sleep_count >= 40){
		if(!is_sleep){
			g1 -> sleep = true;
			g2 -> sleep = true;
			is_sleep = true;
		}else{
			g1 -> sleep = false;
			g2 -> sleep = false;
			g1 -> caught = false;
			g2 -> caught = false;
			is_sleep = false;
			new_t();
		}
		sleep_count = 0;
	}
	if(is_sleep) new_t();
	path = find_path(current_node);
	if(path != NULL){
		node *p = path -> p;
		if(p != NULL){
			px = p -> x;
			py = p -> y;
		}else{
			px = 9;
			py = 8;
			new_t();
		}
	}
	if(grid[py][px] == '2'){
		score += 10;
		cout << "score = " << score << " life = " << life << endl;
		del_pillist(px, py);
		if(pillist == NULL){
			grid[py][px] = '1';
			glutPostRedisplay();
			cout << "\nfinished !!!" << endl;
			exit(0);
		}
	}
	grid[py][px] = '1';
	if(px == g1 -> x && py == g1 -> y && g1 -> caught == false){
		if(!is_sleep){
			glutPostRedisplay();
			cout << "lost one life !!!" << endl;
			sleep(2);
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			
			life--;
			cout << "score = " << score << " life = " << life << endl;
			
		}else{
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			g1 -> caught = true;
			new_t();
			score += 100;
			cout << "score = " << score << " life = " << life << endl;
		}
	}
	
	if(px == g2 -> x && py == g2 -> y && g2 -> caught == false){
		if(!is_sleep){
			glutPostRedisplay();
			cout << "lost one life !!!" << endl;
			sleep(2);
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			
			life--;
			cout << "score = " << score << " life = " << life << endl;

		}else{
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			g2 -> caught = true;
			new_t();
			score += 100;
			cout << "score = " << score << " life = " << life << endl;
		}
	}
	if(!g1 -> caught){
		path_g1 = find_path2(current_node_g1, txg1, tyg1);
		if(path_g1 != NULL){
			node *p = path_g1 -> p;
			g1 -> x = p -> x;
			g1 -> y = p -> y;
		}
	}
	if(!g2 -> caught){
		path_g2 = find_path2(current_node_g2, txg2, tyg2);
		if(path_g2 != NULL){
			node *p = path_g2 -> p;
			g2 -> x = p -> x;
			g2 -> y = p -> y;
		}

	}
	if(px == g1 -> x && py == g1 -> y && g1 -> caught == false){
		if(!is_sleep){
			glutPostRedisplay();
			cout << "lost one life !!!" << endl;
			sleep(2);
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			
			life--;
			cout << "score = " << score << " life = " << life << endl;
			
		}else{
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			g1 -> caught = true;
			new_t();
			score += 100;
			cout << "score = " << score << " life = " << life << endl;
		}
	}
	
	if(px == g2 -> x && py == g2 -> y && g2 -> caught == false){
		if(!is_sleep){
			glutPostRedisplay();
			cout << "lost one life !!!" << endl;
			sleep(2);
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			current_node_g1 -> x = g1 -> x = 8;
			current_node_g1 -> y = g1 -> y = 9;
			
			life--;
			cout << "score = " << score << " life = " << life << endl;

		}else{
			current_node_g2 -> x = g2 -> x = 10;
			current_node_g2 -> y = g2 -> y = 9;
			g2 -> caught = true;
			new_t();
			score += 100;
			cout << "score = " << score << " life = " << life << endl;
		}
	}
	if(px == tx && py == ty){
		new_t();
	}
	if(g1 -> x == txg1 && g1 -> y == tyg1){
		new_t_g1();
	}
	if(g2 -> x == txg2 && g2 -> y == tyg2){
		new_t_g2();
	}
	if(counter >= 30){
		counter = 0;
		new_t_g1();
		new_t_g2();
	}
	free(current_node);
	free(current_node_g1);
	free(current_node_g2);
	glutPostRedisplay();
	if(f) glutTimerFunc(tpm, myIdleFunc, 0);
}
void keyboard(unsigned char key, int x, int y){
	if(key == 27) {		
		exit(0);	
	}else if((char)key == 'p'){
		if(!f){ 
			glutTimerFunc(tpm, myIdleFunc, 0);
			f = true;
		}else
			f = false;
	}else if((char)key == 'e'){
		exit(0);
	}
}
void init(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);	
	glLoadIdentity ();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	GLfloat acolor[] = {1.4, 1.4, 1.4, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, acolor);
}
void copy_file(){
	int i = 0;
	int j = 0;

	ifstream myReadFile;
	myReadFile.open("grid2");
	char c;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> c;
			if(j == 19){
				i++;
				j = 0;
			}else{
				if(c == '0'){
					add_pillist(j, i);
					grid[i][j] = '2';
				}	
				else if(c == '1') grid[i][j] = '0';
				else grid[i][j] = '1';	
				j++;
			}
		}
	}
	myReadFile.close();
}
void Reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(45.0, (float)w/(float)h, 0.1, 200.0);
}
int main(int argc, char** argv){
	srand(time(NULL));
	
	cout << "-----------------------------------------------" << endl;
	cout << endl;
	cout << "               --- PAC-MAN A.I. ---            " << endl;
	cout << endl;
	cout << "created by : tharindra galahena (inf0_warri0r) " << endl;
	cout << "blog       : www.inf0warri0r.blogspot.com      " << endl;
	cout << endl;
	cout << "-----------------------------------------------" << endl;
	cout << endl;
	cout << endl;
	
	
	g1 = (ghost *)malloc(sizeof(ghost));
	g1 -> x = 7;
	g1 -> y = 9;
	g1 -> caught = false;
	g1 -> sleep = false;
	
	g2 = (ghost *)malloc(sizeof(ghost));
	g2 -> x = 11;
	g2 -> y =  9;
	g2 -> caught = false;
	g2 -> sleep = false;
	
	copy_file();
	pill *p = pillist;
	int i = 0;
	while(p != NULL){
		p = p -> next;
	}
	new_t();
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700,700);
	glutInitWindowPosition(500,0);
	glutCreateWindow("pac-man A.I.");
	
	init();
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	
	glutMainLoop();
	return 0;
}



