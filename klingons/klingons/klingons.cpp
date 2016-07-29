// klingons.cpp : Trekkie game by Alex Baranov.
// 2016, SPb, Russia.
// Version 1

// TODO improvement: Init whole galaxy at startup. All sectors.
// TODO adjust sector and galaxy map outputs (coordinate axis) and inputs due to SECTOR and GALAXY 
// TODO feature Remember each sector's stars and other objects.
// TODO feature: Planets. Revolving around the star every 4 actions - moving or shooting.
// TODO feature: Star names. Generated.
// TODO feature: Stardust.
// TODO feature: Asteroids. Like planets, but wanering in space.
// TODO feature: Constellations. Unique star configurations per sector or adjacent sectors.
// TODO Long range radar radius of 3-4 squares. Detects klingons. Bases are generated at start locations known.
// TODO rewrite: Modularize program (.h, several .cpp).
// TODO feature: Make sense with "Clingons rest" on the final screen

// -------------
// TODO WIP Release2: Global goal. Make all stats (fuel, oxygen, energy) useful.
// TODO feature: Clingon boarding mode. Ability to capture things and be attacked in return.
// TODO feature: Text quests, prompt personalization and randomization.
// TODO feature: Localizations.
// TODO feature: Galaxy jump events (drive falure).
// TODO feature: Ship's subsystems.

/*
. Win32
. DOS
. Linux
. OS X
. Android

. Google Play

. Win 8.1 store
. Win 10 universal
. Win mobile 10

. iOS

. Steam
. Flash
*/

#include "stdafx.h"
#include <stdlib.h> 
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>

//using namespace std;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#define PI 3.14159265

int clingons = 0;
int ship_x = 0;
int ship_y = 0;

// Local sector objects. 
// 0 - space
// 1 - star
// 2 - clingon
// 3 - our ship
// 4 - base
// 5 - warhead
#define SECTOR 10
int sector[SECTOR][SECTOR] = { 0 };

// Global galaxy map
// 1 - base
// 2 - ship
// 3 - visited
#define GALAXY 12
int galaxy[GALAXY][GALAXY] = { 0 };

int init_sector(int stars_min, int stars_max, int clingons_min, int clingon_max, bool base) {

	int j;
	int k;

	srand(time(NULL));

	for (int i = 0; i < SECTOR; i++) {
		for (int j = 0; j < SECTOR; j++) {
			sector[i][j] = 0;
		}
	}


	ship_x = 0;
	ship_y = 0;
	// stars
	for (int i = 1; i <= (rand() % (stars_max - stars_min) + stars_min); i++) {
		do {
			j = rand() % SECTOR;
			k = rand() % SECTOR;
		} while (sector[j][k] != 0);
		sector[j][k] = 1;
	}
	// clingons
	for (int i = 1; i <= (rand() % (clingon_max - clingons_min) + clingons_min); i++) {
		do {
			j = rand() % SECTOR;
			k = rand() % SECTOR;
		} while (sector[j][k] != 0);
		sector[j][k] = 2;
		clingons++;
	}
	// Our ship
	do {
		ship_x = rand() % SECTOR;
		ship_y = rand() % SECTOR;
	} while (sector[ship_x][ship_y] != 0);
	sector[ship_x][ship_y] = 3;
	// 0-1 base
	if (base == true) {
		do {
			j = rand() % SECTOR;
			k = rand() % SECTOR;
		} while (sector[j][k] != 0);
		sector[j][k] = 4;
	}

	return 0;
}

int draw_statusbar(int energy, int warheads, float fuel, float oxygen) {
	system("CLS");
	cout << "Energy: " << energy <<
		"    Warheads storage: " << warheads <<
		"    Fuel level: " << std::setprecision(4) << fuel << "%" <<
		"    Oxygen: " << std::setprecision(5) << oxygen << "t" << endl;
	return 0;
}

int draw_galaxy(int energy, int warheads, float fuel, float oxygen, int galaxy[GALAXY][GALAXY], string prompt) {
	draw_statusbar(energy, warheads, fuel, oxygen);
	cout << endl << "Known galaxy map:" << endl <<
		"------------------------------------------" << endl;
	cout << "|     0  1  2  3  4  5  6  7  8  9 10 11 |" << endl;
	for (int i = 0; i < GALAXY; i++) {
		cout << "|";
		if (i < 10) { cout << " "; }
		cout << " 0" << i;
		for (int j = 0; j < GALAXY; j++) {
			switch (galaxy[i][j]) {
			case 1:
				cout << " B ";
				break;
			case 2:
				cout << "-S-";
				break;
			case 3:
				cout << " . ";
				break;
			default:
				cout << " ? ";
				break;
			}
		}
		cout << "|" << endl;
	}
	cout << "------------------------------------------" << endl;
	cout << prompt;
	//system("pause");
	return 0;
}

int draw_sector(int energy, int warheads, float fuel, float oxygen, int sector[SECTOR][SECTOR], string prompt, int mode) {
	draw_statusbar(energy, warheads, fuel, oxygen);
	// sector map
	cout << endl << "Near range radar screen:" << endl <<
		"-----------------------------------" << endl;
	cout << "|    0  1  2  3  4  5  6  7  8  9 |" << endl;
	for (int i = 0; i < SECTOR; i++) {
		cout << "| 0" << i;
		for (int j = 0; j < SECTOR; j++) {
			switch (sector[i][j]) {
			case 1:
				cout << " * ";
				break;
			case 2:
				cout << " k ";
				break;
			case 3:
				cout << "-S-";
				break;
			case 4:
				cout << " B ";
				break;
			case 5:
				cout << " w ";
				break;
			default:
				cout << " . ";
				break;
			}
		}
		cout << "|";
		if (mode == 0) {
			if (i == 3) { cout << "  Available commands:"; }
			if (i == 4) { cout << "          a - attack with warhead"; }
			if (i == 5) { cout << "          m - move"; }
			if (i == 6) { cout << "          j - jump to another sector"; }
			if (i == 7) { cout << "          q - quit"; }
		}
		if (mode == 1) {
			if (i == 1) { cout << "  Moving directions:"; }
			if (i == 3) { cout << "              8   1   2"; }
			if (i == 5) { cout << "              7   S   3"; }
			if (i == 7) { cout << "              6   5   4"; }
		}
		if (mode == 2) {
			if (i == 1) { cout << "     Warhead course:"; }
			if (i == 3) { cout << "             315  0  45"; }
			if (i == 5) { cout << "             270  S  90"; }
			if (i == 7) { cout << "             225 180 135"; }
		}
		if (mode == 3) {
			if (i == 3) { cout << "  Available commands:"; }
			if (i == 4) { cout << "          a - attack with warhead"; }
			if (i == 5) { cout << "          m - move"; }
			if (i == 6) { cout << "          d - dock"; }
			if (i == 7) { cout << "          j - jump to another sector"; }
			if (i == 8) { cout << "          q - quit"; }
		}
		cout << endl;
	}
	cout << "-----------------------------------" << endl;
	cout << prompt;

	return 0;
}

int intro() {
	//char ch;

	system("CLS");
	cout << "-----------------------------------------------------\n";
	cout << "              Old Console Trekkie Game\n\n";
	cout << "            (Not like Xbox or PS console)\n";
	cout << "\n";
	cout << "                     Remake\n";
	cout << "                      v.01\n";
	cout << "\nFYI some radar readings:\n\n";
	cout << " * - star. Hot ball of hot gases. Quite hot, indeed.\n";
	cout << " k - slimy klingon. Seven-assed enemy.\n";
	cout << " S - our ship. She's real beauty, ain't she, capt'n?\n";
	cout << " B - sector base. Supplies!\n";
	cout << " w - dangerous warhead.\n";
	cout << " . - vast black space. No air, just starsine.\n";
	cout << "\nYou could wander around, launch warhead occasionaly\n";
	cout << "and quit whenever you like. Shiny! Also jumping\n";
	cout << "between galaxy sectors is not prohibited. Yet.\n";
	cout << "-----------------------------------------------------\n";
	cout << "\n";
	cout << "\n";
	system("pause");
	return 0;
}

int endgame(int clingons, bool base_destroyer) {
	system("CLS");
	cout << "----------------------------------------\n";
	cout << "-Clingons rest: " << clingons << endl;
	if (base_destroyer == true) {
		cout << "-By the way, you are wanted fugitive now\n for destroying friendly base.\n";
		cout << "-Your own base, for god's sake!\n";
	}
	cout << "-See ya, capt'n!\n";
	cout << "----------------------------------------\n\n\n";
	system("pause");
	return 0;
}

int main()
{

	int warheads = 15;
	float fuel = 100;
	float oxygen = 5000;
	int energy = 8300;
	int j = 0;
	int k = 0;
	// command menu
	// a - attack
	// m - move
	// d - dock
	// j - jump
	// q - quit
	char input = 'z';
	string prompt = "\n\n-Your orders, capt'n?\n-";
	int ship_dest_x = 0;
	int ship_dest_y = 0;
	// Draw prompt_mode
	// 0 - avaliable commands
	// 1 - moving direction
	// 2 - attack routine
	// 3 - docking
	int prompt_mode = 0;
	int warhead_course = 0;
	double warhead_x = 0;
	double warhead_y = 0;
	double delta = 0.01;
	double warhead_radius = 1;
	int target = 0;
	bool docking = false;
	int warhead_old_x = 0;
	int warhead_old_y = 0;
	bool base_destroyer = false;
	bool quit = false;
 
	vector<string> prompts;

	int galaxy_x = 11;
	int galaxy_y = 6;

	// randomize sector : 6-11 stars, 1-4 clingons, 1 ship, 0-1 base
	init_sector(6, 11, 1, 4, true);

	galaxy[galaxy_x][galaxy_y] = 2; // Ship in the right sector

	intro();

	prompts.push_back("\n\n-We are short on warheads, need resupply!\n-What should we do?\n-"); //0
	prompts.push_back("\n\n-Warhead course?\n-"); //1
	prompts.push_back("\n\n-Warhead wanished in the star's flames!\n-Orders?\n-"); //2
	prompts.push_back("\n\n-Clingon killed!\n-What should we do now?\n-"); //3
	prompts.push_back("\n\n-Base destroyed!\n-What's next? Alderaan?\n-"); //4
	prompts.push_back("\n\n-Lost warhead telemetry!\n-More space garbage in the area!\n-"); //5
	prompts.push_back("\n\n-Wrong bearing, sir!\n-Your command, sir?\n-"); //6
	prompts.push_back("\n\n-Heading, capt'n?\n-"); //7
	prompts.push_back("\n\n-Can't carry the order, sir!\n-Your command, sir?\n-"); //8
	prompts.push_back("\n\n-Out of range, sir!\n-Orders?\n-"); //9
	prompts.push_back("\n\n-Star ahead!\n-Orders, sir?\n-"); //10
	prompts.push_back("\n\n-Clingon ahead!\n-Attacking, sir?\n-"); //11
	prompts.push_back("\n\n-Base ahead!\n-We can dock, sir?\n-"); //12
	prompts.push_back("\n\n-Something ahead!\n-What will be the orders, sir?\n-"); //13
	prompts.push_back("\n\n-Maneuver commenced!\n-Orders, sir?\n-"); //14
	prompts.push_back("\n\n-Your orders, capt'n?\n-"); //15
	prompts.push_back("\n\n-Got fresh ones!!\n-New orders, sir?\n-"); //16
	prompts.push_back("\n\n-Whats the move, cap?\n-"); //17
	prompts.push_back("\n\n-New heroic orders, capt'n?\n-"); //18
	prompts.push_back("\n\n-Awaiting orders, capt'n?\n-"); //19
	prompts.push_back("Out of gas, cap! Neen refill!"); //20
	prompts.push_back("\n\n-We've jumped right in the target, capt'n! Yew!\n-Scanning sector..."); //21
	prompts.push_back("-Wrong coordinates, nav officer's drunk!\n"); //22
	prompts.push_back(" Gotcha!"); //23
	prompts.push_back(" Success!"); //24
									//prompts.push_back(); //25
									//prompts.push_back(); //26
									//prompts.push_back(); //27
									//prompts.push_back(); //28
									//prompts.push_back(); //29

	do
	{
		switch (input) {
		case 'q': // Quit
			endgame(clingons, base_destroyer);
			quit = true;
			break;
		case 'a': // Attack
			prompt_mode = 2;
			if (warheads == 0) {
				prompt = prompts[0];
				break;
			}
			prompt = prompts[1];
			draw_sector(energy, warheads, fuel, oxygen, sector, prompt, prompt_mode);

			if (std::cin >> warhead_course)
			{
				// It worked (input is now in a good state)

				warheads--;
				warhead_course = 180 - warhead_course;
				if (warhead_course < 0) { warhead_course += 360; }
				warhead_old_x = ship_x;
				warhead_old_y = ship_y;
				warhead_radius = 1;
				do {
					warhead_radius += delta;
					warhead_x = cos(warhead_course * PI / 180) * warhead_radius;
					warhead_y = sin(warhead_course * PI / 180) * warhead_radius;
					if ((ship_x + (int)round(warhead_x)) >= 0 &&
						(ship_y + (int)round(warhead_y)) >= 0 &&
						(ship_x + (int)round(warhead_x)) <= 9 &&
						(ship_y + (int)round(warhead_y)) <= 9) {
						target = sector[ship_x + (int)round(warhead_x)][ship_y + (int)round(warhead_y)];
					}
					else { break; }
					if (target == 0 &&
						(
						(ship_x + (int)round(warhead_x) != warhead_old_x) ||
							(ship_y + (int)round(warhead_y) != warhead_old_y)
							)
						) {
						sector[ship_x + (int)round(warhead_x)][ship_y + (int)round(warhead_y)] = 5;
						draw_sector(energy, warheads, fuel, oxygen, sector, prompt, prompt_mode);
						_sleep(500);
						sector[ship_x + (int)round(warhead_x)][ship_y + (int)round(warhead_y)] = 0;
						warhead_old_x = ship_x + (int)round(warhead_x);
						warhead_old_y = ship_y + (int)round(warhead_y);
					}
					if ((target != 0) && (target != 3)) {
						if (target != 1) {
							sector[ship_x + (int)round(warhead_x)][ship_y + (int)round(warhead_y)] = 0;
						}
						break;
					}
				} while ((abs(warhead_x) <= (float)SECTOR) && (abs(warhead_y) <= (float)SECTOR));

				//warhead_old_x = ship_x;
				//warhead_old_y = ship_y;
				switch (target) {
				case 1:
					prompt = prompts[2];
					break;
				case 2:
					prompt = prompts[3];
					clingons--;
					break;
				case 4:
					prompt = prompts[4];
					base_destroyer = true;
					break;
				default:
					prompt = prompts[5];
					break;
				}

			}
			else
			{
				// input is in a bad state.
				// So first clear the state.
				cin.clear();

				// Now you must get rid of the bad input.
				// Personally I would just ignore the rest of the line
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// now that you have reset the stream you can go back and try and read again.
				prompt = prompts[6];
			}

			break;
		case 'm': // Move
			prompt = prompts[7];
			prompt_mode = 1;
			draw_sector(energy, warheads, fuel, oxygen, sector, prompt, prompt_mode);
			cin >> input;
			switch (input)
			{
			case '7':
				ship_dest_x = ship_x;
				ship_dest_y = ship_y - 1;
				break;
			case '6':
				ship_dest_x = ship_x + 1;
				ship_dest_y = ship_y - 1;
				break;
			case '5':
				ship_dest_x = ship_x + 1;
				ship_dest_y = ship_y;
				break;
			case '4':
				ship_dest_x = ship_x + 1;
				ship_dest_y = ship_y + 1;
				break;
			case '3':
				ship_dest_x = ship_x;
				ship_dest_y = ship_y + 1;
				break;
			case '2':
				ship_dest_x = ship_x - 1;
				ship_dest_y = ship_y + 1;
				break;
			case '1':
				ship_dest_x = ship_x - 1;
				ship_dest_y = ship_y;
				break;
			case '8':
				ship_dest_x = ship_x - 1;
				ship_dest_y = ship_y - 1;
				break;
			default:
				prompt = prompts[8];
				break;
			}
			if ((input >= '1') && (input <= '8')) {
				if ((ship_dest_x < 0) || (ship_dest_y < 0) || (ship_dest_x > 9) || (ship_dest_y > 9)) {
					prompt = prompts[9];
				}
				else
					if (sector[ship_dest_x][ship_dest_y] != 0) {
						switch (sector[ship_dest_x][ship_dest_y]) {
						case 1:
							prompt = prompts[10];
							break;
						case 2:
							prompt = prompts[11];
							break;
						case 4:
							prompt = prompts[12];
							docking = true;
							break;
						default:
							prompt = prompts[13];
							break;
						}
					}
					else {
						sector[ship_x][ship_y] = 0;
						sector[ship_dest_x][ship_dest_y] = 3;
						ship_x = ship_dest_x;
						ship_y = ship_dest_y;
						prompt = prompts[14];
						if (fuel >= (float)3.1) { fuel -= (float)3.1; }
						else { fuel = 0; }
						if (oxygen >= (float)5.3) { oxygen -= (float)5.3; }
						else { oxygen = 0; }
						if (energy >= 700) { energy -= 700; }
						else { energy = 0; }
					}
			}
			break;
		case 'd':
			if (docking == false) {
				prompt = prompts[15];
				break;
			}
			warheads = 15;
			fuel = 100;
			if (oxygen < 5000) { oxygen = 5000; }
			if (energy < 8300) { energy = 8300; }
			docking = false;
			prompt = prompts[16];
			break;
		case 'j':
			prompt = prompts[17];
			draw_galaxy(energy, warheads, fuel, oxygen, galaxy, prompt);
			cin >> ship_dest_y >> ship_dest_x;
			if (fuel < 4) { cout << prompts[20]; }
			else {
				if (ship_dest_x >= 0 &&
					ship_dest_x <= 11 &&
					ship_dest_y >= 0 &&
					ship_dest_y <= 11) {

					galaxy[galaxy_x][galaxy_y] = 3; // Visited space "."
					galaxy[ship_dest_x][ship_dest_y] = 2;

					galaxy_x = ship_dest_x;
					galaxy_y = ship_dest_y;

					cout << "Moving to " << ship_dest_y << ":0" << ship_dest_x << "...";
					_sleep(2000);
					cout << prompts[23] << endl;
					_sleep(1000);
					fuel -= 4;
					prompt = prompts[21];
					draw_galaxy(energy, warheads, fuel, oxygen, galaxy, prompt);
					_sleep(2000);
					cout << prompts[24] << endl;

					// randomize sector : 5-20 stars, 1-5 clingons, 1 ship, 0-1 base
					init_sector(5, 20, 1, 5, (((rand() % 2) == 0) ? (true) : (false)));
					if (fuel >= (float)17.1) { fuel -= (float)17.1; }
					else { fuel = 0; }
					if (oxygen >= (float)10.3) { oxygen -= (float)10.3; }
					else { oxygen = 0; }
					if (energy >= 5700) { energy -= 5700; }
					else { energy = 0; }

				}
				else {
					cout << prompts[22];
				}
			}
			_sleep(2000);
			prompt = prompts[18];
			//	init_sector((rand() % 1 + 13), (rand() % 20 + 15), 1, 4, true);
			break;
		default:
			prompt = prompts[19];
			break;
		}
		prompt_mode = 0;
		if (docking == true) { prompt_mode = 3; }
		if (quit == true) { break; }
		draw_sector(energy, warheads, fuel, oxygen, sector, prompt, prompt_mode);
		if (oxygen >= (float)0.9) { oxygen -= (float)0.9; }
		else { oxygen = 0; }
		if (fuel >= (float)0.01) { fuel -= (float)0.01; }
		else { fuel = 0; }
		if (energy <= 19750) { energy += 250; }
		else { energy = 20000; }
	} while (cin >> input);

	return 0;
}