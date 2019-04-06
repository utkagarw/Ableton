#include <iostream>
#include <string>
#include <fstream> 
#include <map>
#include <sstream> 

using namespace std;

ifstream infile("input.txt"); //File for reading the input
ofstream txtOut; //printing output to output.txt file

class clip{  // class for depicting clip
public :

	enum actions{none, any, other, next, previous};  // enum for denoting actions associated with a clip


	// Constructor of the class
	clip(string &clipName, double tick, double chance1, double chance2, actions act1, actions act2, double time1 = 1, double time2 = 1){
		name = clipName;
		ticks = tick;
		followChance1 = chance1;
		followChance2 = chance2;
		action1 = act1;
		action2 = act2;
		playAction1 = time1;
		playAction2 = time2;
	}

	// method for inserting new clips 
	// It is a static method so that it can be called from anywhere
	static void insertClip(clip *clipId, map<int, clip*> &clipStore, int &totalClips){ 
		totalClips++;
		string clipName = clipId->name;
		for(map<int, clip*>::iterator iter = clipStore.begin(); iter != clipStore.end(); iter++){
			if(clipName == iter->second->name){
				int index = iter->first;
				clipStore.erase(iter->first); // logic for handling the case when the clip name matches any other previous clip name
				clipStore[index] = clipId;   // replace the previous clip with the new clip
				totalClips--;
				return;
			}
		}
		clipStore[totalClips] = clipId;
		return;
	}

	string name;  // stores clip name
	double ticks; // stores clip ticks
	double followChance1; // stores chance of 1 action
	double followChance2; // stores chance of 2 action
	actions action1; // enum denotes 1 action
	actions action2; // enum denotes 2 action
	double playAction1; // count of how many times action 1 has been performed
	double playAction2; // count of how many times action 2 has been performed
};


// function for populating enum in a map for reading
void populateEnum(map<string, clip::actions> &enumMap){
	enumMap["none"] = clip::none;
	enumMap["any"] = clip::any;
	enumMap["next"] = clip::next;
	enumMap["other"] = clip::other;
	enumMap["previous"] = clip::previous;
}

map<int, clip*> clipStore; //stores the clip
int totalClips = 0; // stores total clips
int currentClip = 1; // denotes which clip is playing
map<string, clip::actions> enumMap; // enum map


// function for generating random numbers for "any" and "other" actions
int randomGenerator(bool var, int index){
	int number = rand();
	if(var == true)return (number%totalClips)+1;
	else{
		if((number%totalClips)+1 == index)randomGenerator(false, index);
		else return (number%totalClips)+1;
	}
	return 1;
}

// helper function for finding which indexed clip to play next
void modifyIndex(clip::actions act, int &index){
	//corner case when the total number of clips is 1
	if(totalClips == 1)
		return;
	
	switch(act){
		case clip::none :
			break;

		case clip::any :
			index = randomGenerator(true, index);
			break;

		case clip::other :
			index = randomGenerator(false, index);
			break;

		case clip::next :
			if(index == totalClips)index = 1;
			else index++;
			break;

		case clip::previous :
			if(index == 1)index = totalClips;
			else index--;
			break;
	}
	return; 
}

//  main function for finding which indexed clip needs to be played next
void playNext(int &index, map<int, clip*> &clipStore){
	clip::actions act;
	double c1 = clipStore[index]->followChance1;
	double c2 = clipStore[index]->followChance2;
	double &p1 = clipStore[index]->playAction1;
	double &p2 = clipStore[index]->playAction2;

	// Logic for finding next action that needs to be taken associated with a clip
	// Here, proper averages are maintained
	if(c1/p1 > c2/p2){
		act = clipStore[index]->action1;
		p1++;
	}
	else{
		act = clipStore[index]->action2;
		p2++;
	}
	
	return modifyIndex(act, index);
}

// function for playing the clip
void playClip(int &index, map<int, clip*> &clipStore, double count){
	int i=1;
	if(clipStore.find(index) == clipStore.end())return; // if index is not present then return the function
	int iter = clipStore[index]->ticks; // finding the ticks of the clip that is going to be played
	string name = clipStore[index]->name; // finding the name of the clip that is going to be played
	while(i <= (int)count){
		if(clipStore.find(index) == clipStore.end())return;
		if(iter > 0){
			txtOut << name << endl; // logic for playing the clip iter number of times
			iter--;
		}
		if(iter == 0){
			playNext(index, clipStore);
			iter = clipStore[index]->ticks; // finding the ticks of the clip that is going to be played
			name = clipStore[index]->name; // finding the name of the clip that is going to be played
		}
		i++;
	}
}

int main(){
	std::string line;
	txtOut.open ("output.txt"); //opening the output file
	populateEnum(enumMap); // Here mapping an enum value to a string and storing it in a map

	while (std::getline(infile, line) /*reading input line by line */)
	{
	    istringstream iss(line); 
	    
	    string token;

	    string clipName;
	    double tick, chance1, chance2, tickCount;
	    string action1, action2;


	    if(!(iss>>token))continue; // if the line is blank. then continue
	    
	    if(token == "clip"){ // first word of the line is clip then add the clip
	    	if(!(iss >> clipName >> tick >> chance1 >> chance2 >> action1 >> action2))continue;
	    	// checking if the action passed is a valid action or not
	    	if(enumMap.find(action1) == enumMap.end() || enumMap.find(action2) == enumMap.end())continue; 
	    	clip::actions act1 = enumMap[action1]; 
	    	clip::actions act2 = enumMap[action2];
	    	clip * newClip = new clip(clipName, tick, chance1, chance2, act1, act2);
	    	clip::insertClip(newClip, clipStore, totalClips);
	    }

	    if(token == "ticks"){ // first word of the line is ticks then print the clip 
	    	if(!(iss >> tickCount))continue;
	    	playClip(currentClip, clipStore, tickCount); // play the clips for the ticks
	    }

	    else continue; // if none of the above then continue

	}
	return 1;
}