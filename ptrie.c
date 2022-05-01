#include "ptrie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct ptrie_s {
    // NULL or the string that ends here
    char *this;     
    // Max count of me and all children, my count        
    int max, cnt;            
    // Pointer to the array of children
    struct ptrie_s* child;        
} ptrie_t;

struct ptrie {
    // Simple array of structures for every letter
    ptrie_t array[256];      
};


struct ptrie *ptrie_allocate(void) {
    // Allocate 256 structures and set to NULL/0
    void *vp = calloc( 256, sizeof( ptrie_t ) );        

    if ( vp == NULL ) 
    {
        perror( "ptrie_allocate calloc" );
    }

    return vp;
}

void ptrie_free(struct ptrie *pt) {
    int i;

    // Nothing to do
    if ( pt == NULL ) 
    {                     
        return;
    }
        
    for ( i = 0; i < 256; i++ ) 
    {           
        // Free the children first by recursive calls
        ptrie_free( (struct ptrie*)(pt->array[i].child) );         
        // Free any copied strings
        free( pt->array[i].this );          
    }

    // Free the parent
    free( pt );                            
}

// Recursive helper for add, returns max for this character or -1 for error
static int ptrie_add_h(ptrie_t *pt, const char *str, int level) {
    int ret;

    // String ends here
    if ( str[level + 1] == '\0' ) 
    {         
        // First time for this string
        if ( pt[(int)str[level]].this == NULL ) 
        {     
            // Get a 'malloc'd copy of the string
            if ( NULL == ( pt[(int)str[level]].this = strdup( str ) ) ) 
            {      
                perror( "ptrie_add strdup" );
                return -1;
            }
        } 

        // Update count and compare to max
        if ( ++pt[(int)str[level]].cnt > pt[(int)str[level]].max ) 
        {        
            // Update max
            pt[(int)str[level]].max = pt[(int)str[level]].cnt;              
        }

        // Return this character's max
        return pt[(int)str[level]].max;       
    } 

    // Need a child node
    if ( NULL == pt[(int)str[level]].child ) 
    {        
        // Create empty child node 
        if ( NULL == ( pt[(int)str[level]].child = calloc( 256, sizeof( ptrie_t ) ) ) ) 
        {         
            perror( "ptrie_add calloc" );
            return -1;
        }
    }

    // Add the next letter
    ret = ptrie_add_h( pt[(int)str[level]].child, str, level + 1 );           

    // Is this a new max for this character?
    if ( ret > pt[(int)str[level]].max ) 
    {        
        // Update the max
        pt[(int)str[level]].max = ret;            
    }

    // Return any new max or error
    return ret > 0 ? pt[(int)str[level]].max : ret;       
}

int ptrie_add(struct ptrie *pt, const char *str) {
    if ( *str == '\0' ) 
    {
        return 0;
    } 
    
    return 0 > ptrie_add_h( pt->array, str, 0 ) ? -1 : 0;
}

// Recursive auto_complete helper
static char *ptrie_ac_h( ptrie_t *pt, const char *str) {
    int i;

    // Last character of key
    if ( *str && ! str[1] ) 
    {
        // str is in the tree
        if ( pt[(int) *str].this != NULL && pt[(int) *str].cnt == pt[(int) *str].max ) 
        {
            return NULL;
        }
    }

    // NULL character at end of key
    if ( ! *str ) 
    {     
        // Find the first max and its index
        int max = -1, max_i = -1;       

        // Examine every letter value for the max
        for ( i = 0; i < 256; i++ ) 
        {       
            // Found a new max
            if ( pt[i].max > max ) 
            {      
                // Update the max  
                max = pt[i].max;            
                // Update the max index
                max_i = i;                  
            }
        }

        // Does max apply to this
        if ( max == pt[max_i].cnt ) 
        {   
            // Yes, return this    
            return pt[max_i].this;          
        } 

        // max applies to a child
        return ptrie_ac_h( pt[max_i].child, str );      
    }

    // Prefix is not in tree
    if ( pt[(int)*str].child == NULL ) 
    {        
        return NULL;
    }

    // Go on to the next character
    return ptrie_ac_h( pt[(int)*str].child, str + 1 );        
}

char *ptrie_autocomplete(struct ptrie *pt, const char *str) {
    char * cp;

    // Return a strdup of what was found or str if nothing found
    void *vp = strdup( ( cp = ptrie_ac_h( pt->array, str ) ) == NULL ? str : cp );

    if ( vp == NULL ) 
    {
        perror( "ptrie_autocomplete strdup" );
    }

    return vp;
}

// Recursive helper for in-order print
static void ptrie_print_h(ptrie_t *pt) {
    int i;

    for ( i = 0; i < 256; i++) 
    {
        if ( pt[i].cnt ) 
        {
            printf( "%s\n", pt[i].this );
        }

        if ( pt[i].child != NULL ) 
        {
            ptrie_print_h( pt[i].child );
        }
    }
}

void ptrie_print(struct ptrie *pt) {
    ptrie_print_h( pt->array );
}

