#include<iostream>
#include<string>
#include<vector>
#include<cstdlib>
using namespace std;

class prototype{ //abstract class
    public:
    virtual prototype *clone()const =0;   //*clone(). this method returns a pointer to new obj created if we dont use it slicing will happen in the obj of derived class when cloned and assigned to bass class obj.
    virtual void show()const=0; // we made it in prototype cuz other class can override it and implement it as they want to ensure consistency
    virtual ~prototype()=default;
};

class base_player:public prototype{
    string player_name;
    string initial_weapon;
    const int health;

    public:
    static int player_count;
    base_player(string player_name="",string initial_weapon="pistol",int health=100):player_name(player_name),initial_weapon(initial_weapon),health(health){
        player_count++;
        if(!(player_count<=1)){
        this->player_name = "BOT" + to_string(player_count-1); // after creation of base player all player future creation will be named bot.
        this->initial_weapon="pistol";
        
        }
    }
    base_player *clone()const override{ // here we kept const so that the current object can't be updated
    return new base_player(player_name,initial_weapon,health);
    }
    int get_health()const {
    return health;
    }
    void set_player_name(string player_name){
      this->player_name=player_name;
    }
    void set_weapon(string initial_weapon){
        this->initial_weapon=initial_weapon;
    }
    string get_weapon(){
        return initial_weapon;
    }
    string get_player(){
        return player_name;
    }
    
    void show() const override{  //as it dont change anything we can keep it const and it also work for const objects
        cout<<"Your Name: "<<player_name<<endl;
        cout<<"Your Initial Weapon: "<<initial_weapon<<endl;
        cout<<"Your Health: "<<health<<endl;
    }
    ~base_player()override{
    player_count--;
    }
};
int base_player::player_count=0;

// This is mediator that is a centerlized for the interation of obj without directly to each other but through another mediator.
class room:public prototype{
    int code;
    public:
    int room_space;
    vector<base_player*>playerobj;
    static vector<room*>roomobj;
    static int room_count;

    room(int room_space=0,int code=0){
    this->room_space=room_space;
    this->code=code;
    roomobj.push_back(this);
    room_count++;
    
    }
    int get_code(){
        return code;
    }
    void set_code(int code){
      this->code=code;
    }
    void set_space(int room_space){
      this->room_space=room_space;
    }
    int get_space(){
        return room_space;
    }
    void show()const override{
        cout<<"ALL ROOMS CODE: "<<endl;
    for(auto it:roomobj){
        cout<<it->get_code()<<endl;
    }
    }
    room* clone()const override{      // it is const as the current obj can't be changed mistakenly
    return new room(room_space,code); // also you can change here return type of this clone method it wil be still overriding as parameters and behaviour is still same but c++ allows covariant
    // types overriding where you have two return type options. and this still follows overriding rules. if you dont do that just type cast the returned clonned obj
    // in main if you want to any different type for your use. use dynamic_cast<room*> 
    }

    void static delete_room(int c){
        int found=0;
     for(auto it=roomobj.begin();it!=roomobj.end();){
        
      if((*it)->get_code()==c){
       delete *it;
       it=roomobj.erase(it); // this erase return valid iterator but loop still hold the invalid if i dont assign the valid iterator to it here.
       cout<<"ROOM HAS BEEN DELETED: "<<endl;
       found=1;
       room_count--;
       break;
      }
      else{
        ++it;
      }
    }
    if(!found){
     cout<<"ROOM WAS NOT FOUND: "<<endl;
    }
    }

    int static get_count(){
    return room_count;
    }

    ~room()override{  // this is called at the moment when delete it executes in delete method. and this remove all the room players.
        for(auto &it:playerobj){
            delete it;
            cout<<base_player::player_count<<endl;
        }
    playerobj.clear();
    base_player::player_count=0;
    
    }

};
int room::room_count=0;
vector<room*>room::roomobj;

class connector{ // player and room connector
    public:
    void connect(int c,base_player *player){
      int found=0;
      for(auto &it:room::roomobj){
       if(it->get_code()==c){
        if(it->room_space>0){
        it->playerobj.push_back(player);
        cout<<"ROOM HAS BEEN JOINED: "<<endl;
        it->room_space--;
        found++;
        break;
        }
        else{
            cout<<"room space full"<<endl;
        }
        
        
       }
    }
    if(found==0){
        cout<<"NO CODE MATCHED: "<<endl;
    }
    }
    
    bool find_player_in_room(int code,string name){
     for(auto it:room::roomobj){
        if(it->get_code()==code){
            for(auto itt:it->playerobj){
                if(itt->get_player()==name){
                    return true;
                }
            }
        }
     }
     return false;
    }

    bool current_room_space(int code){
              for(auto it:room::roomobj){
                 if(it->get_code()==code){
                    if(it->room_space>0){
                        return true;
                    }
                    break;
                 }
              }
        return false;
    }
};

class player_manager{
    public:
    void kill_player(string name="",string baseplayername=""){
        int found =0;

    for(auto &it:room::roomobj){ // &it why not it only? cuz &it deals with original room modification not copy.

        auto &player=it->playerobj;  // &player will hold orginal playerobj vector of each room.

     for(auto itt=player.begin();itt!=player.end();itt++){ // itt is iterator and *itt is orignal player in playerobj. player.begin means first player in playerobj vector.

        if((*itt)->get_player()==name){
            delete *itt; // deleted the original player in playerobj vector
            itt=player.erase(itt); // removed the iterator where *itt (original player was present)
            cout<<baseplayername<<" KILLED "<<name<<endl;
            found++;
            it->room_space++;
            break;
        }
     }
    }
    if(base_player::player_count==1){
        cout<<"YOU WON!! , ALL PLAYERS DIED."<<endl;
    }
    if(found==0){
        cout<<" THERE IS NO PLAYER NAMED: "<<name<<endl;
    }
    }

    void clear_current_room_players(int code){
        int found=0;
       for(auto &it:room::roomobj){
        if(it->get_code()==code){

            auto &player=it->playerobj; // we making this because in next loop we using itt which is iterator and when we use erase we need iterator not the object as &it is object not iterator.
            for(auto itt=player.begin();itt!=player.end();){ // no increment here as erase will handle by self cuz manual forloop increment here can cuz issues and crash.
                
                delete *itt; // this is the original player deleted inside the player vector.
                itt=player.erase(itt); //itt is iterator. *itt is the object at index itt.
                found=1;
                if(found==1){ //room space increased by 1
                it->room_space++;
                found=0;
            }
            }
            break;
        }
     }
    }

    void show_current_room_players(int code,int kill_call=0,string base_player_name=""){
       for(auto it:room::roomobj){

        if(it->get_code()==code){
            for(auto itt:it->playerobj){
                if(kill_call==0){
                 cout<<itt->get_player()<<endl;
                }
                else if(kill_call==1){
                    if(itt->get_player()!=base_player_name){
                        cout<<itt->get_player()<<endl;
                    }
                }
            }
            break;
        }
     }
    }

    void show_current_room_players_details(int code){
       for(auto it:room::roomobj){
        if(it->get_code()==code){
            for(auto itt:it->playerobj){
            itt->show();
            cout<<"-----------------------------"<<endl;
            }
          }
       }
    }

    bool kill_yourself(string baseplayername=""){
        string option,botname;
        int code,location;

     for(auto &it:room::roomobj){
        auto &player=it->playerobj;

       if(base_player::player_count!=1){

        for(auto itt=player.begin();itt!=player.end();){
        if((*itt)->get_player()==baseplayername){
            code=it->get_code();
            location=rand()%(player.size()-1)+1; //base_player::player_count-1 this decrease range by 1 so that dont show outofrange. as starting from 1

            botname=player.at(location)->get_player();
            delete *itt;
        itt=player.erase(itt); // when you erase the element from vector like here you need to assign the valid iterator returned by erase to itt.
            it->room_space++;
            cout<<"AWW '"<<botname<<"' KILLED YOU!!, YOU LOST!."<<endl;
            clear_current_room_players(code);
            base_player::player_count=0;
            return true;
           }
         }
       }
       else{
        cout<<"YOU WON!, JOIN NEW ROOM"<<endl;
        break;
       }
       
    }
    return false;
    }

    bool if_player_empty(){
        if(base_player::player_count==0){
            return true;
        }

        return false;
    }
    
};

void game_menu(){
    
    player_manager manager;
    connector connect;
    base_player *player;
    room *r1;
    base_player *bot;
    room *newroom;
    int option,code,space,current_room=0,player_clear=0;
    string initial_name="",weapon="",killbot;
    while(1){
    cout<<"*************************************WELCOME TO THE GAME OF OPTIONS*******************************************"<<endl;
    cout<<"1) Create Room: "<<endl;
    cout<<"2) Join Room: "<<endl;
    cout<<"3) Delete Room: "<<endl;
    cout<<"4) Exit: "<<endl;
    cin>>option;
    switch(option){
        case 1:
        if(room::room_count!=0){ // cloning of base room
        newroom=r1->clone();
        cout<<"SET ROOM CODE: "<<endl;
        cin>>code;
        cout<<"SET ROOM SPACE: "<<endl;
        cin>>space;
        newroom->set_code(code);
        newroom->set_space(space);
        }
        else{ //first room creation
        r1=new room();
        cout<<"SET ROOM CODE: "<<endl;
        cin>>code;
        cout<<"SET ROOM SPACE "<<endl;
        cin>>space;
        r1->set_code(code);
        r1->set_space(space);
        }
        cout<<"YOUR ROOM HAS BEEN CREATED: "<<endl;
        if(initial_name==""){ //firstly  player obj will be created if condition is true
            continue;
        }
        break;

        case 2:
        if(initial_name==""){ // Means base player will be created for the first time
            player=new base_player;
            cout<<"ENTER YOUR DISPLAY NAME: "<<endl;
            cin>>initial_name;
            player->set_player_name(initial_name);
            cout<<"YOUR INITIAL WEAPON IS : (PISTOL)\nDO YOU WANT TO CHANGE IT ?\n1) YES\n2) NO "<<endl;
            cin>>option;
            if(option==1){
             cout<<"ENTER YOUR NEW WEAPON: "<<endl;
             cin>>weapon;
             player->set_weapon(weapon);
            }
        }
        else if(player_clear==1 && initial_name!=""){ //creation of base player after leaving room
            player=new base_player; // NOt bot but the base player is created again after removing all the players from current after pressing leave room.
            player->set_player_name(initial_name);
            if(weapon!=""){
            player->set_weapon(weapon);
            }
        }

        if(room::room_count!=0){ //r1->get_code()!=0
        r1->show(); //all rooms
        cout<<"ENTER ROOM CODE: "<<endl;
        cin>>code;
        current_room=code;
        connect.connect(current_room,player);
        }
        else{
            cout<<"THERE IS ARE NO ROOMS!, CREATE THE ROOM FIRST."<<endl;
            continue;
        }
        break;
        
        case 3:
        room::roomobj[0]->show();
        cout<<"ENTER ROOM CODE: "<<endl;
        cin>>code;
        room::delete_room(code);
        break;

        case 4:
        cout<<"THANKS FOR PLAYING!! "<<endl;
        delete bot;
        delete player;
        delete newroom;
        delete r1;
        break;

        default:
        cout<<"INCORRECT OPTION SELECTED!. "<<endl;
        continue;
    }

     if(connect.find_player_in_room(current_room,initial_name)!=true){ //validation even the player has joined or not.
       continue;
    }

    bool again=true;
    while(again!=false){
       cout<<"1) SHOW ALL PLAYERS IN ROOM: "<<endl;
       cout<<"2) SHOW ALL PLAYER DETAILS: "<<endl;
       cout<<"3) ADD BOT IN ROOM: "<<endl;
       cout<<"4) PLAY: "<<endl;
       cin>>option;

        switch(option){
            case 1:
            manager.show_current_room_players(current_room);
            break;
            
            case 2:
            manager.show_current_room_players_details(current_room);
            break;

            case 3:
            if(connect.current_room_space(current_room)==true){
            bot=player->clone();
            connect.connect(current_room,bot);
            }
            else{
                cout<<"SORRY, ROOM SPACE IS NOW FULL"<<endl;
            }
            break;

            case 4:
            again=false;
            break;
        }
       }

       again=true;
       while(again!=false){
        cout<<"1) KILL PLAYER: "<<endl;
        cout<<"2) GET KILLED BY A BOT: "<<endl;
        cout<<"3) LEAVE ROOM: "<<endl;
        cin>>option;

        switch(option){
        case 1:
        
        if(base_player::player_count!=1){
        manager.show_current_room_players(current_room,1,initial_name);
        cout<<"ENTER BOT NAME TO KILL: "<<endl;
        cin>>killbot;
        manager.kill_player(killbot,initial_name);
        }
        else{
            cout<<"ALL PLAYERS DIED! ,JOIN A NEW ROOM: "<<endl;
        }
        break;

        case 2: 
        if(manager.kill_yourself(initial_name)){
        again=false;
        player_clear=1;
        }
        break;

        case 3:
        manager.clear_current_room_players(current_room);
        again=false;
        player_clear=1;
        break;
       }

      }

     } //<- main while loop end

    } // <- Function end

int main(){
    game_menu();
    
    return 0;
}