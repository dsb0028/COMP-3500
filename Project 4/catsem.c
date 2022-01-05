/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
 * this fs file.
 */


/*
 * Includes
 *  
 *     
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>

/*
 * 
 * Constants
 *    
 */

/*
 * Number of loops
 */

#define NUM_LOOPS 3

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 *  Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2

/*
 * Total number of animals
 */

#define TOTAL_ANIMALS 8

/*
 * defining true to be 1
 * defining false to be 0
 */
#define true 1
#define false 0

/*
 * 
 *  Definitions
 * 
 */


/* 
 * Semaphore done    
 * Keeps track of the number of cats or mice who are done eating
 */
static struct  semaphore *cats_done;

static struct  semaphore *mice_done;
/*
 * Semaphore mutex
 * Controls access to the kitchen 
 */
static struct  semaphore *mutex;
/*
 * Semaphore dish_mutex
 * Controls access to dishes   
 */
static struct semaphore *dish_mutex; 
/*
 * Semaphore cats_queue
 * Keeps track of the number of cats who are waiting to eat
 */
static struct  semaphore *cats_queue; 
/*
 * Semaphore mice_queue
 * Keeps track of the number of mice who are waiting to eat
 */
static struct  semaphore *mice_queue;
/*
 * Semaphore all_done
 * Keeps track of the number of cats and mice who are done eating
 */
static struct  semaphore *all_done;
/*
 * _Bool all_dishes_available
 *  A variable of type volatile _Bool that indicates whether all dishes are available or not
 *  True -  no animal, cat or mouse, is occupying a dish
 *  False - At least one cat or mouse is occupying a dish
 *  Initalized to be true since there are no animals eating before the simulation starts, implying that there can't be dishes that are busy. 
 */     
static volatile _Bool all_dishes_available = true;
/*
 * cats_wait_eat_count
 * A variable of type volatile int that indicates how many cats are waiting in the queue and eating at any given time.
 * Initialized to be zero since there are initially no cats eating and waiting. 
 */
static volatile  int cats_wait_eat_count = 0;

/*
 * no_cat_eat
 * A variable of type volatile _Bool that indicates whether there are cats eating or not
 * True - there are no cats eating
 * False - there is at least one cat eating
 * Initialized to be true since there are no cats eating initially.
 */
static volatile _Bool no_cat_eat = true;
/*
 * no_mouse_eat
 * A variable of type volatile _Bool that indicates whether there are mice eating or not
 * True - there are no mice eating
 * False - there is at least one mouse eating
 * Initialized to be true since there are no mice eating initally.
 */
static volatile _Bool no_mouse_eat = true;
/*
 * dish_1_busy
 * A variable of type volatile _Bool that indicates whether dish1 is being 
 * occupied or not.
 *
 * True - dish1 is being occupied by either a cat or mouse
 * False - dish1 is not being occupied 
 */
static volatile _Bool dish1_busy;
/*
 * dish_2_busy
 * A variable of type volatile _Bool that indicates whether dish2 is being
 * occupied or not.
 *
 * True - dish2 is being occupied
 * False - dish2 is not being occupied
 */
static volatile _Bool dish2_busy;

/*
 * mice_wait_eat_count
 * A variable of type volatile int that represent the number of mice who are waiting 
 * to enter the kitchen and are currently eating
 * Initialized to be zero because there are initially zero mice eating and waiting
 */
static volatile  int mice_wait_eat_count = 0;
 /*
 * first_cat_eat
 * A variable of type volatile _Bool that indicates whether the first cat to enter the 
 * kitchen has eaten or not
 *
 * True - first cat to enter kitchen has eaten
 * False - the first cat to enter the kitchen has not eaten yet
 */
//static volatile _Bool first_cat_eat = false;
/*
 * another_cat_eat 
 * A variable of type volatile _Bool that indicates whether a non-first cat 
 * to enter the kitchen has eaten or not.
 *
 * True - non-first cat has eaten
 * False - non-first cat has not eaten
 */
//static volatile _Bool another_cat_eat = false;
/*
 * first_mouse_eat
 * A variable of type volatile _Bool that indicates whether the first mouse to enter
 * the kitchen has eaten or not
 *
 * True - first mouse to enter kitchen has eaten
 * False - first mouse to enter kitchen has not eaten 
 */
//static volatile _Bool first_mouse_eat = false;
/*
 * another_mouse_eat
 * A variable of type volatile _Bool that indicates whether a non-first mouse 
 * to enter the kitchen has eaten or not
 *
 * True - non-first mouse to enter the kitchen has eaten
 * False - non-first mouse has not eaten yet
 */
//static volatile _Bool another_mouse_eat = false;

/*HELPER FUNCTIONS */

/*
 * INITIALIZE_ALL_SEMS()
 * Parameters: none 
 * Return: nothing
 * Notes: all semaphores that were previously declared are initialzed
 * If a semaphore is NULL after creating it, then the kernel will panic due to
 * being out of memory.
 */

void INITIALIZE_ALL_SEMS(void) {
    cats_done = sem_create("cats_done", 0);
     if (cats_done == NULL) {
          panic("Cannot create sempaphore cats_done because OS161 is out of memory\n");
     } 

    mice_done = sem_create("mice_done", 0);
    if (mice_done == NULL) {
          panic("Cannot create sempaphore mice_done because OS161 is out of memory\n");
    }

   all_done = sem_create("all_done", 0);
     if (all_done == NULL) {
          panic("Cannot create sempaphore all_done because OS161 is out of memory\n");  

     }

    mutex = sem_create("mutex", 1);
     if (mutex == NULL) {
          panic("Cannot create sempaphore mutex because OS161 is out of memory\n");     
    }

    dish_mutex = sem_create("dish_mutex", 1);
     if (dish_mutex == NULL) {
          panic("Cannot create sempaphore dish_mutex because OS161 is out of memory\n");     
    }
    
    cats_queue = sem_create("cats_queue", 0);
     if (cats_queue == NULL) {
          panic("Cannot create sempaphore cats_queue because OS161 is out of memory\n");      
    }
    mice_queue = sem_create("mice_queue", 0);
     if (mice_queue == NULL) {
          panic("Cannot create sempaphore mice_queue because OS161 is out of memory\n");      
    }
   
   P(mutex);
   dish1_busy = false;
   dish2_busy = false;
   V(mutex);
    
}

/*
 * CLEANUP_ALL_SEMS()
 * Parameters: none
 * Return: nothing
 * Notes: destroys all semaphores that have been previously created
 */

void CLEANUP_ALL_SEMS(void) {
 sem_destroy(cats_done);
 sem_destroy(mice_done); 
 sem_destroy(all_done);
 sem_destroy(mutex);
 sem_destroy(dish_mutex);
 sem_destroy(cats_queue);
 sem_destroy(mice_queue);
}



/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
   _Bool first_cat_eat;
   _Bool another_cat_eat;
/*
 * Simulates cats eating and playing NUM_LOOPS times. 
 *   
 */
 int i;
 for(i = 0; i< NUM_LOOPS; i++) {
       clocksleep(2); /*play outside the kitchen */
        /* kitchen is locked by the first cat that is let into the kitchen */
        P(mutex);
	if (all_dishes_available == true) {
	    all_dishes_available = false;
	    V(cats_queue);
	}
        
        /* The number of cats that are waiting and eating will be incremented to 1 
         * to reflect that the first cat enter the kitchen.
         */
     	 cats_wait_eat_count++;
         /*  Kitchen is unlocked by the first cat */
	  V(mutex);
	 /*Other cats waiting to enter the kitchen are forced to continue waiting*/
	 P(cats_queue);	  
         
         /*
          * If no cat has eaten yet, it must be the first cat's turn to eat, 
          * else it is another cat's turn to eat
          */
         
	 if (no_cat_eat == true) {
	  no_cat_eat = false;
	  first_cat_eat = true;
	 }
	 
	 else {
	  first_cat_eat = false;
	 }
         
          /* 
           *  If it is first cat's turn to eat, then the kitchen will be locked again.
           *  Then if there are at least two cats waiting and eating, another cat must also want to eat
           *  and will be let into the kitchen.
           *  After both cats are let into the kitchen, the kitchen will be unlocked. 
           */
          
	  if (first_cat_eat == true) {
	   P(mutex);
	   if (cats_wait_eat_count > 1) {
	     another_cat_eat = true;
	     V(cats_queue);
	   }
	   V(mutex);
	  }
          
 
          P(mutex);
	  kprintf("Cat %lu is in the kitchen.\n", catnumber);
          V(mutex);	
	 /*
          * my dish is a local variable handled by catsem 
          * Access to the dishes are secured and if the first dish is not being occupied,
          * then the first dish will be assigned. 
          * Else if the second dish is not occupied, the second dish will be assigned
          * Afterwards the dishes are no longer secure.
          */
          
	 P(dish_mutex);
         int mydish;
	 if (dish1_busy == false) {
	  dish1_busy = true;
          P(mutex);
          kprintf("Cat %lu is now at dish 1.\n", catnumber);
          V(mutex);
	  mydish = 1;
	  }
	  
	 else {
	  assert(dish2_busy == false);
          dish2_busy = true;
          P(mutex);
          kprintf("Cat %lu is now at dish 2.\n", catnumber);	
          V(mutex);  
          mydish = 2; 	 
	 }
	 V(dish_mutex);
          
         /*
          * Cat eats for one second and then indicates that they are finished eating
          */
         P(mutex);
	 kprintf("Cat %lu starts eating.\n", catnumber);
	 clocksleep(1); 
	 kprintf("Cat %lu is finished eating.\n", catnumber);
	 V(mutex);
         /*
          * If the first dish is occupied, then it will be returned.
          * Else the second dish must be occupied and will subsequently be returned.
          */
         
	 P(dish_mutex);
	 if (mydish == 1) {
                P(mutex);
                kprintf("Cat %lu is returning dish 1.\n", catnumber);
                V(mutex);
		dish1_busy = false;	 
	 }
	 
	 else {
                P(mutex);
                kprintf("Cat %lu is returning dish 2.\n", catnumber);
                V(mutex);
		dish2_busy = false;
	 }
	 V(dish_mutex);
         
	 
         /*
          * The number of cats waiting and eating is decremented by one to reflect a cat that is no longer eating
          */
	 P(mutex);
	 cats_wait_eat_count--;
	 V(mutex);	 
          /* If the first cat and non-first cat have finished eating, the second cat watches the first cat will leave the kitchen. Now the second cat feels extremely lonely. 
           * Access to the kitchen is made available to waiting cats and mice.
           * Essentially, after the first two cats leave the kitchen, mice that are currently waiting to to enter the kitchen will get their opportinuty.
           * We don't want poor Minney and Mickey to starve, even though there are still four more cats that have not eaten. 
           *
           * Also, if the first cat is finished eating and has left the kitchen, the non-first cat will notify the first cat that they are also leaving.
           * Once the first two cats have finished eating and have left the kitchen, 
           * waiting mice will be given an opportunity over waiting cats if there are any. 
           */
              
	 if (first_cat_eat == true) {
	   if (another_cat_eat == true) {
	        P(cats_done);
                P(mutex);
		kprintf("Cat %lu is leaving.\n", catnumber);
                V(mutex);
		no_cat_eat = true;
	    
		P(mutex);		
		if(mice_wait_eat_count > 0) {
			V(mice_queue);
                }

		else {
			if(cats_wait_eat_count > 0) {
				V(cats_queue);
			}
			else {
				all_dishes_available = true;
			}	 
	       }  
            		 
	     V(mutex);
	   }
          	    
	 }
	 
	 else {
                P(mutex);
		kprintf("Cat %lu is leaving.\n", catnumber);
                V(mutex);

		V(cats_done);	 
	 }	

    } 
    
  /* Cats are done eating and playing like good kittens */
    P(mutex);
    kprintf("Cat %lu finishes eating and playing.\n", catnumber);
    V(mutex);

    V(all_done);

   
   // (void)unusedpointer; 
   // (void)catnumber;  
}

static 
void 
mousesem(void * unusedpointer,
       unsigned long mousenumber)
 {

  _Bool first_mouse_eat;
  _Bool another_mouse_eat;
  int i;
 /*
  * Simulates mice eating and playing NUM_LOOPS times.  
  */

  for(i = 0; i < NUM_LOOPS; i++) {
       clocksleep(2); /*play outside the kitchen */
       /* kitchen is locked by the first mouse that is let into the kitchen */                                     
   	 P(mutex);
	if (all_dishes_available == true) {
		all_dishes_available = false;
	    V(mice_queue);
	}
         /* The number of mice that are waiting and eating will be incremented to 1 
          * to reflect that the first mouse to enter the kitchen.
          */
     	 mice_wait_eat_count++;
          /*  Kitchen is unlocked by the first mouse */
	 V(mutex);
	  /* now that the first mouse is in the kitchen, the other mouse will have to wait*/
	 P(mice_queue);	 
          /* If no mouse has eaten yet, it must be the first mouses's turn to eat
           * Else it is the second mouse's turn to eat. 
           */
	 if (no_mouse_eat == true) {
	  no_mouse_eat = false;
	  first_mouse_eat = true;
	 }
	  
	 else {
	  first_mouse_eat = false;
	 }   
           /*
            * If it is the first mouse's turn to eat, then the kitchen will be locked again.
            * Then if there are at least two mice waiting and eating, the second mouse can now take his turn and will subsequently enter the kitchen.
            * Now both mice are in the kitchen and the lock to it is made available. 
            */ 
	  if (first_mouse_eat == true) {
	   P(mutex);
	   if (mice_wait_eat_count > 1) {
	     another_mouse_eat = true;
	   V(mice_queue); /* let second mouse in */
	   }
            /* lock is made available (kitchen is unlocked) */ 
	    V(mutex);
	  }
	  /* Mouse 1 or Mouse 2 is in the kitchen */
          P(mutex);
	  kprintf ("Mouse %lu is in the kitchen.\n", mousenumber);	
          V(mutex);
	  /*
          * my dish is a local variable handled by mousesem 
          * Access to the dishes are secured and if the first dish is not being occupied,
          * then the first dish will be assigned. 
          * Else if the second dish is not occupied, the second dish will be assigned
          * Afterwards the dishes are no longer secure.
          */ 

	 
	 P(dish_mutex);
         int mydish;
	 if (dish1_busy == false) {
	  dish1_busy = true;
          P(mutex);
          kprintf("Mouse %lu is now at dish 1.\n", mousenumber);
          V(mutex);
	  mydish = 1;
	  }
	  
	 else {
	  assert(dish2_busy == false);
          dish2_busy = true;
          P(mutex);	 
          kprintf("Mouse %lu is now at dish 2.\n", mousenumber); 
          V(mutex);
          mydish = 2; 	 
	 }
	 V(dish_mutex);
         /*
          * Mouse eats for one second and then indicates that they are finished eating
          */
         P(mutex);
	 kprintf("Mouse %lu starts eating.\n", mousenumber);
	 clocksleep(1); 
	 kprintf("Mouse %lu is finished eating.\n", mousenumber);
         V(mutex);
	 /*
 	  * If the first dish is occupied it will be returned.
	  * Else the second dish must be occupied and will also be returned. 
 	  * It may have been wise to assert that the second dish has been busy already, because
	  * if it isn't with the first dish being released, there will be an error.
 	  * It is not efficient nor practical to to have either one cat or one mouse enter the kitchen, eat, and then exit the kitchen without the second dish every being occupied by other cat or mouse.
	  * The policy is there must be social distancing of the animals due to COVID-19
	  * Cats and Mice cannot be in the kitchen at the same time. If Tom and Jerry were real animals, you would not want Tom nor Jerry to get sick and die, otherwise there would not be a TV show.   
 	  */
	 P(dish_mutex);
	 if (mydish == 1) {
            P(mutex);
            kprintf("Mouse %lu is returning dish 1.\n", mousenumber);
            V(mutex);
	    dish1_busy = false;	 
	 }
	 
	 else {
              P(mutex);
              kprintf("Mouse %lu is returning dish 2.\n", mousenumber);
              V(mutex);
	      dish2_busy = false;
	 }
	 V(dish_mutex);
	 /*
          * The number of mice waiting and eating is decremented by one to indicate that a dish has just been relinquished by an animal and to show that the animal is no longer eating.
          */
	 P(mutex);
	 mice_wait_eat_count--;
	 V(mutex);	 
         /*
          * If both the first mouse and second mouse have finished eating, the second mouse  watches the first mouse leave the kitchen. In other words, Mickey is now sad that Minney has now left the kitchen.  
          * Essentially, after the first two mice leave the kitchen, cats that have not gotten their opportinity to enter the kitchen and eat yet will now have their chance.
          * Even though Tom and his annoying cat buddies may have been mean to Mickey and Minney, they still need to be able to eat. We don't want dead cats waiting outside the kitchen. 
	  * Also, if the first mouse is finished eating and has left the kitchen, the second will follow suit and notify the first mouse that they are also leaving the kitchen.       
          */
         	
	 if (first_mouse_eat == true) {
	   if (another_mouse_eat == true) {
	    P(mice_done);
                P(mutex);
		kprintf("Mouse %lu is leaving.\n", mousenumber);
                V(mutex);
		no_mouse_eat = true;
	    
		P(mutex);
		
		if(cats_wait_eat_count > 0) {
			V(cats_queue);
                }  

		else {
			if(mice_wait_eat_count > 0) {
				V(mice_queue);
			}
			else {
				all_dishes_available = true;
			}	 
	       }
		 
	       V(mutex);
	   }
           	    
	 }
	 
	 else {
                P(mutex);
		kprintf("Mouse %lu is leaving.\n", mousenumber);
                V(mutex);

		V(mice_done);	 
	 }	
    }

    P(mutex);
    kprintf("Mouse %lu finishes eating and playing.\n", mousenumber);
    V(mutex);

    V(all_done);
   // (void)unusedpointer; 
    //(void)mousenumber; 
 }


/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
         
         /*
          * All semaphores along with dish1_busy and dish2_busy are initialized 
          */
         INITIALIZE_ALL_SEMS();
 
   
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
               
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /* wait until all animals are done */
     int i;
     for (i = 0; i < TOTAL_ANIMALS; i++) { /*TOTAL_ANIMALS = 8*/  
        P(all_done); 
     }
      /* clean up  */
    CLEANUP_ALL_SEMS();
      // (void) nargs;
       //(void) args;

        return 0;
}


/*
 * End of catsem.c
 */
