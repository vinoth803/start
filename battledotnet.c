#include "common.h"
#include "battledotnet.h"
#include "threads.h"

#define MATRIX_ROW_INDEX 10
#define MATRIX_COL_INDEX 10
#define MATRIX_POISON     5

static int player_count = 1;
char   player_char_count = 'A';


struct node *head;
/* This function assigns the players Name
 * based on Alpha-numeric combinations 
 */
char * getplayername() {
    char *name;
    
    if(player_char_count == 'Z') {
        player_char_count = 'A';
        player_count++;        
    }
    
    name = malloc(sizeof(player_count)+sizeof(player_char_count));
    sprintf(name,"%c%d",player_char_count, player_count);
    player_char_count++;
    return name;
}
/* This function populates the dotMatrix with 
 * POSIN value of "5" to confirm, wheather the 
 * particuler index is hit or miss
 */
void populate_dotMatrix(int arr[10][10]) {
    int i,j;
    for(i=0;i<MATRIX_ROW_INDEX;i++) {
        for(j=0;j<MATRIX_COL_INDEX;j++) 
            arr[i][j] = MATRIX_POISON;
    }
}
/* This function sets ship position
 * randomly based on rand() function
 */
void setDotship(int arr[10][10]) {
    int i,j;
    i = rand() % 10;
    j = rand() % 10;
    BDN_log("[%s:%10d] setting index of (%d, %d)",__func__,__LINE__,i,j);
    arr[i][j] = 1;
}

/* Displays DotMatrix of the Node,
 * used for debugging purpose
 */
void display_dotMatrix(int arr[10][10]) {
    int i,j;
    for(i=0;i<MATRIX_ROW_INDEX;i++) {
        for(j=0;j<MATRIX_COL_INDEX;j++) 
            BDN_log("%3d", arr[i][j]);
        
        BDN_log("\n");
    }
}
/* This function is called by thread function
 * for adding a new player in the system
 * It is called once for a player. 
 * Nodes pName is mapped against the thread 
 * and thread_id is updated in the node
 * This is basically addition of node
 * to a circular list
 */
char * include_player(pthread_t tid) {

	struct node* new_node;

	new_node = (struct node*)malloc(sizeof(struct node));
  	new_node->playName  = getplayername();
    populate_dotMatrix(new_node->dotmatrix);
    setDotship(new_node->dotmatrix);
	new_node->thread_id = tid;
    
	if (head == NULL) {
		new_node->next      = new_node;
		head                = new_node;
	}
	else {
		new_node->next      = head->next;
		head->next          = new_node;
		head                = new_node;
	}

	return new_node->playName;
}

/* Wrapper function of free */
void bdn_free(struct node *trace) {
	free(trace);
}

/* This function removes a player from the 
 * game, when it setDotShip is destoryed by
 * its opponents. 
 * This is deletion of node from a circular
 * list
 */
int remove_player(char *pName) {

	struct node 	*temp, *trace = head, *prev = NULL;
	int 		rc=0;
	int		count = 0;
	if (head == NULL) {
		BDN_log("[%s:%10d] List is empty",__func__,__LINE__);
		return (-1);
	}

	viewList();
	do { 
		if(strcmp(trace->playName,pName) == 0) {
			break;
		}
		
		prev = trace;
		trace = trace->next;
		count++;
	}while( trace != head);
	
	if(head == trace) {
		temp = head;
		while(temp->next != trace){
			prev=temp;
			temp=temp->next;
		}
		prev=temp;
		head = head->next;
		prev->next=head;
		viewList();
	}
 	if(prev != trace && prev != NULL ) {
		prev->next = trace->next;
	}
		
	BDN_log("[%s:%10d] Deleting player %s",
			__func__, __LINE__, trace->playName);
	bdn_free(trace);
	viewList();
	return 0;
}

/* This is a helper funtion, according to the 
 * game rule Player A opponent is Player B
 * Adjacent players are the opponents 
 * so if pName is passed oppName is returned 
 */ 
char * findopponent(char *pName) {

	struct node *temp;
	char        *str;
	temp = head->next;
	if(head == NULL || pName == NULL) {
		BDN_log("[%s:%10d] List is empty",__func__,__LINE__);
		return NULL;
	}
	else {
		do {
			if((strcmp(pName,temp->playName) == 0)) {
				str=temp->next->playName;
				break;
			}
			temp = temp->next;
		} while (temp != head);
	}

	if(strcmp(temp->playName,pName) == 0)
		str=temp->next->playName;

	BDN_log("[%s:%10d] Player Name : %s Opp Player Name %s",
			__func__,__LINE__,pName,str);
	return str;
}

/* This is a helper function
 * Confirms wheather the given node is present
 * in the circular list
 */
bool isNodePresent(char *pName) {
	if (head == NULL && pName == NULL ) {
		BDN_log("[%s:%10d] List is empty",__func__, __LINE__);
		return false;
	}		
	else { 
		struct node* temp;
		temp = head->next;
		do {
			if(strcmp(temp->playName,pName) == 0) {
				return true;
			}
			temp = temp->next;
		} while (temp != head->next);

		if(strcmp(temp->playName,pName) == 0) {
			return true;
		}
		else {
			return false;
		}
	}
}

/* This function helps in checking the
 * game play, whether a particular co-ordinates
 * is hitting its target or not.
 */
int bombPlayer(char *pName, int i, int j) {

	struct node *temp;

	if(head == NULL || pName == NULL) {
		BDN_log("[%s:%10d] List or player Name is empty",__func__,__LINE__);
		return 1;
	}
	else {
		temp = head->next;
		do {
			if(strcmp(pName,temp->playName) == 0) {
				if(temp->dotmatrix[i][j] == 1) {
					BDN_log("[%s:%10d] !.Boooom..! Enemy destoryed",__func__,__LINE__);
					return 1;
				}
				else if(temp->dotmatrix[i][j] == 6) {
					BDN_log("[%s:%10d] !..Already hit co-ordinate",__func__,__LINE__);
					return 0;
				}
				else {
					BDN_log("[%s:%10d] !..OOpps missed it",__func__,__LINE__);
					temp->dotmatrix[i][j]++;
					return 0;
				}

			}
			else
				temp = temp->next;
		} while (temp != head->next);
	}
	return 0;
}
/* Debug function */		
void viewList() {
	if (head == NULL)
		BDN_log("[%s:%10d] List is empty",__func__, __LINE__);

	else {
		struct node* temp;
		temp = head->next;
		do {
			BDN_log("[%s:%10d] Player Name %s",
					__func__, __LINE__, temp->playName);
			//display_dotMatrix(temp->dotmatrix); //uncomment when required
			temp = temp->next;
		} while (temp != head->next);
	}
}

int main()
{
	/* Initialize thread variables */
	thread_init();

	/* Start the thread pool */
	thread_start();

	return 0;
}
