#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// CREATING FLAT AND APARTMENT CLASSES
class Flat {
public:
    string apt_name;
    int id;
    int is_empty;
    int initial_bandwidth;
    Flat* next;
    Flat* prev;
};

class Apartment {
public:
    int max_bandwidth;
    Apartment* next;
    string name;
    Flat* fhead;
};


void add_apartment(Apartment **head, string name, string position, int max_bandwidth){
    // CREATING A APARTMENT OBJECT FOR NEW APARTMENT
    Apartment *newApt = new Apartment();
    newApt->name = name;
    newApt->max_bandwidth = max_bandwidth;

    // CHECKING IF APARTMENT LIST IS EMPTY
    if (*head == NULL){
        *head = newApt;
        newApt->next = newApt;
        return;
    }
    // IF INSERTING AT HEAD
    if (position=="head"){
        newApt->next = *head;
        *head = newApt;
    }
    else {
        // SPLITTING POSITION STRING FOR TO GET BEFORE/AFTER AND APARTMENT NAME
        string pos_arr[2];
        stringstream pos_ss(position);
        string pos;
        int i = 0;
        while (getline(pos_ss, pos, '_')) {
            pos_arr[i] = pos;
            i++;
        }
        // IF INSERTING AT AFTER
        if (pos_arr[0] == "after") {
            Apartment *temp = *head;
            while (temp->name != pos_arr[1]) {
                temp = temp->next;
            }
            newApt->next = temp->next;
            temp->next = newApt;
        }
        // IF INSERTING AT BEFORE
        if (pos_arr[0] == "before") {
            Apartment *temp = *head;
            // IF INSERTING AT HEAD
            if (temp->name == pos_arr[1]) {
                Apartment *tail = *head;
                while (tail->next != *head){
                    tail = tail->next;
                }
                tail->next = newApt;
                newApt->next = *head;
                *head = newApt;

            } else {
                while (temp->next->name != pos_arr[1]) {
                    temp = temp->next;
                }
                newApt->next = temp->next;
                temp->next = newApt;
            }
        }
    }
}

void add_flat(Apartment **head, string apt_name, int index, int initial_bandwidth, int id){

    // CREATING A FLAT OBJECT FOR NEW FLAT
    Flat *newFlat = new Flat();
    newFlat->apt_name = apt_name;
    newFlat->id = id;
    newFlat->is_empty = 0;

    // FINDING APARTMENT
    Apartment *apt = *head;
    while (apt->name != apt_name){ apt = apt->next; }
    Flat *fhead = apt->fhead;

    // CALCULATING NEW FLAT'S INITIAL BANDWIDTH
    int sum_of_bandwidths = 0;
    Flat *current = fhead;
    while (current != NULL){
        sum_of_bandwidths += current->initial_bandwidth;
        current = current->next;
    }
    if (initial_bandwidth <= apt->max_bandwidth - sum_of_bandwidths){
        newFlat->initial_bandwidth = initial_bandwidth;
    } else if (initial_bandwidth > apt->max_bandwidth - sum_of_bandwidths){
        newFlat->initial_bandwidth = apt->max_bandwidth - sum_of_bandwidths;
    } else if (apt->max_bandwidth == sum_of_bandwidths){
        newFlat->initial_bandwidth = 0;
    }
    if (newFlat->initial_bandwidth == 0) newFlat->is_empty = 1;

    // INSERTING NEW FLAT
    if (index == 0) {
        if (fhead == NULL) {
            fhead = newFlat;
        }
        else {
            fhead->prev = newFlat;
            newFlat->next = fhead;
            fhead = newFlat;
        }
    }
    else {
        Flat *temp = fhead;
        for (int i = 0; i < index-1; ++i) {
            temp = temp->next;
        }
        if (temp->next == NULL){
            newFlat->next = NULL;
            temp->next = newFlat;
            newFlat->prev = temp;
        } else {
            temp->next->prev = newFlat;
            newFlat->next = temp->next;
            temp->next = newFlat;
            newFlat->prev = temp;
        }
    }
    Apartment *temp = *head;
    while (temp->name != apt_name){ temp = temp->next; }
    temp->fhead = fhead;
}

void remove_apartment(Apartment **head, string apt_name){
    Apartment *previous = *head;

    // IF REMOVING HEAD
    if (previous->name == apt_name){
        *head = previous->next;
        Flat *current = previous->fhead;
        Flat *next = NULL;
        while (current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
        free(previous);
    } else {
        while (previous->next->name != apt_name) {
            previous = previous->next;
        }
        Apartment *apt = previous->next;
        previous->next = apt->next;
        apt->next = NULL;

        Flat *current = apt->fhead;
        Flat *next = NULL;
        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
        free(apt);
    }
}

void make_flat_empty(Apartment **head, string apt_name, int flat_id){
    // FINDING APARTMENT
    Apartment *apartment = *head;
    while (apartment->name != apt_name){ apartment = apartment->next; }

    // FINDING FLAT
    Flat *flat = apartment->fhead;
    while (flat->id != flat_id){ flat = flat->next; }

    // MAKING FLAT EMPTY
    flat->initial_bandwidth = 0;
    flat->is_empty = 1;
}

void find_sum_of_max_bandwidths(Apartment **head, ofstream *output){
    int sum_of_max_bandwidths = 0;
    Apartment *apartment = *head;

    // TRAVERSING ALL APARTMENTS AND ADDING THEIR MAX BANDWIDTH TO SUM OF MAX BANDWIDTHS
    while (apartment->next != *head){
        sum_of_max_bandwidths += apartment->max_bandwidth;
        apartment = apartment->next;
    } sum_of_max_bandwidths += apartment->max_bandwidth;

    // WRITING SUM OF BANDWIDTHS TO OUTPUT FILE
    *output << "sum of bandwidth: " << sum_of_max_bandwidths << "\n\n";
}

void merge_two_apartments(Apartment **head, string apt1_name, string apt2_name) {
    // FINDING APARTMENTS
    Apartment *apartment1 = *head;
    Apartment *apartment2 = *head;
    while (apartment1->name != apt1_name) { apartment1 = apartment1->next; }
    while (apartment2->name != apt2_name) { apartment2 = apartment2->next; }

    Flat *fhead1 = apartment1->fhead;
    Flat *fhead2 = apartment2->fhead;

    // IF APARTMENT 2'S FLAT LIST IS EMPTY
    if (fhead2 == NULL) {
        // ADDING APARTMENT 2'S BANDWIDTH TO APARTMENT 1
        apartment1->max_bandwidth += apartment2->max_bandwidth;

        // REMOVING APARTMENT 2
        remove_apartment(head, apt2_name);
    }
    // IF APARTMENT 1'S FLAT LIST IS EMPTY
    else if (fhead1 == NULL && fhead2 != NULL) {
        Flat *flat2 = fhead2;
        while (flat2 != NULL) {
            flat2->apt_name = apt1_name;
            flat2 = flat2->next;
        }
        // MOVING APARTMENT 2 TO APARTMENT 1
        fhead1 = fhead2;
        apartment1->max_bandwidth += apartment2->max_bandwidth;
        apartment2->fhead = NULL;
        remove_apartment(head, apt2_name);
    }
    else if (fhead1 != NULL && fhead2 != NULL) {
        Flat *flat2 = fhead2;
        while (flat2 != NULL) {
            flat2->apt_name = apt1_name;
            flat2 = flat2->next;
        }
        // FINDING APARTMENT 1'S TAIL
        Flat *ftail1 = fhead1;
        while (ftail1->next != NULL) { ftail1 = ftail1->next; }

        // MOVING APARTMENT 2 TO APARTMENT 1
        ftail1->next = fhead2;
        fhead2->prev = ftail1;
        apartment1->max_bandwidth += apartment2->max_bandwidth;
        apartment2->fhead = NULL;

        // REMOVING APARTMENT 2
        remove_apartment(head, apt2_name);
    }
}

void relocate_flats_to_same_apartment(Apartment **head, string apt_name, int flat_id_to_shift, string flat_id_list){
    // DELETING BRACES FROM STRING TO BE ABLE TO SPLIT IT
    flat_id_list = flat_id_list.substr(1, flat_id_list.size() - 2);

    // GETTING SIZE OF FLAT ID LIST
    stringstream ss1(flat_id_list);
    string id;
    int ids_count = 0;
    while (getline(ss1,id,',')){ids_count++;}

    // CREATING AN ARRAY TO STORE GIVEN FLAT ID'S
    int* id_list;
    id_list = new int[ids_count];
    stringstream ss2(flat_id_list);
    int i = 0;
    while (getline(ss2,id,',')){
        id_list[i] = stoi(id);
        i++;
    }

    // FINDING APARTMENT
    Apartment *apartment = *head;
    while (apartment->name != apt_name){ apartment = apartment->next; }

    // FINDING FLAT TO SHIFT
    Flat *flat_to_shift = apartment->fhead;
    while (flat_to_shift->id != flat_id_to_shift){ flat_to_shift = flat_to_shift->next; }

    // MOVING EVERY FLAT IN LIST
    for (int j = 0; j < ids_count; j++) {

        // FINDING GIVEN FLAT IN STREET
        int flat_id = id_list[j];
        Apartment *current = *head;
        Flat *flat = current->fhead;
        while (flat->id != flat_id){
            if (flat->next != NULL){
                flat = flat->next;
            } else{
                current = current->next;
                flat = current->fhead;
            }
        }
        flat->apt_name = apartment->name;
        current->max_bandwidth -= flat->initial_bandwidth;
        apartment->max_bandwidth += flat->initial_bandwidth;

        // REMOVING FLAT FROM ITS APARTMENT
        if (flat->prev == NULL && flat->next == NULL){
            current->fhead = NULL;
        } else if (flat->prev == NULL && flat->next != NULL){
            current->fhead = flat->next;
            flat->next = NULL;
        } else if (flat->prev != NULL && flat->next == NULL){
            flat->prev->next = NULL;
            flat->prev = NULL;
        } else {
            flat->prev->next = flat->next;
            flat->prev = NULL;
            flat->next = NULL;
        }

        // MOVING FLAT TO NEW APARTMENT
        if (flat_to_shift == apartment->fhead){
            flat_to_shift->prev = flat;
            flat->next = flat_to_shift;
            apartment->fhead = flat;
        } else {
            flat_to_shift->prev->next = flat;
            flat->prev = flat_to_shift->prev;
            flat_to_shift->prev = flat;
            flat->next = flat_to_shift;
        }
    }
}


void print_apartment(Apartment **head, string apt_name, ofstream *output){
    // FUNCTION TO WRITE AN APARTMENT AND ITS FLATS

    Apartment *apartment = *head;
    while (apartment->name != apt_name){
        apartment = apartment->next;
    }
    *output << apartment->name << "(" << apartment->max_bandwidth << ")";
    if (apartment->fhead != NULL) {
        Flat *flat = apartment->fhead;
        while (flat->next != NULL) {
            *output << "\tFlat" << flat->id << "(" << flat->initial_bandwidth << ")";
            flat = flat->next;
        }
        *output << "\tFlat" << flat->id << "(" << flat->initial_bandwidth << ")";
    }
}

int main(int argc, char** argv) {

    // GETTING NUMBER OF ALL ARGUMENTS IN INPUT FOR STORING THEM IN AN ARRAY
    string command, line;
    int size = 0;
    ifstream inputFile1(argv[1]);
    if (inputFile1.is_open()) {
        while (getline(inputFile1, line)) {
            stringstream ss(line);
            while (getline(ss, command, '\t')) {
                size++;
            }
        }
    }
    // CREATING ARRAY WHICH INCLUDES ALL COMMANDS IN INPUT
    string *commands;
    commands = new string[size];
    ifstream inputFile2(argv[1]);
    if (inputFile2.is_open()) {
        for (int i = 0; i < size; ++i) {
            inputFile2 >> commands[i];
        }
    }
    // CLOSING INPUT FILES
    inputFile1.close();
    inputFile2.close();

    // CREATING HEAD OF APARTMENT LINKED LIST
    Apartment *head = NULL;

    // OPENING OUTPUT FILE
    ofstream output(argv[2], ios::out);

    if (output.is_open()) {

        // CHECKING COMMANDS AND CALL ITS FUNCTION FOR EACH COMMAND
        for (int i = 0; i < size; ++i) {
            if (commands[i] == "add_apartment") {
                add_apartment(&head, commands[i + 1], commands[i + 2],
                              stoi(commands[i + 3]));
            } else if (commands[i] == "add_flat") {
                add_flat(&head, commands[i + 1], stoi(commands[i + 2]),
                         stoi(commands[i + 3]), stoi(commands[i + 4]));
            } else if (commands[i] == "remove_apartment") {
                remove_apartment(&head, commands[i + 1]);
            } else if (commands[i] == "make_flat_empty") {
                make_flat_empty(&head, commands[i + 1], stoi(commands[i + 2]));
            } else if (commands[i] == "find_sum_of_max_bandwidths") {
                find_sum_of_max_bandwidths(&head, &output);
            } else if (commands[i] == "merge_two_apartments") {
                merge_two_apartments(&head, commands[i + 1], commands[i + 2]);
            } else if (commands[i] == "relocate_flats_to_same_apartment") {
                relocate_flats_to_same_apartment(&head, commands[i + 1], stoi(commands[i + 2]), commands[i + 3]);
            } else if (commands[i] == "list_apartments") {
                Apartment *apartment = head;
                while (apartment->next != head) {
                    print_apartment(&head, apartment->name, &output);
                    output << "\n";

                    apartment = apartment->next;
                }
                print_apartment(&head, apartment->name, &output);
                output << "\n\n";
            }
        }
    }
    // CLOSING OUTPUT FILE AND FREEING MEMORY OF HEAD
    output.close();
    free(head);
}
