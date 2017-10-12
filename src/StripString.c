/*---------------------------------------------------------------------
C-  Function:   StripString.c
C-
C-  Purpose/    StripString is called to remove (strip) leading and
C-  Methods:    trailing blanks/tabs and the final carriage return
C-              from character stings read using the fgets command.
C-
C-
C-  Inputs:     *s  (character string passed from calling routine)
C-
C-  Outputs:    *s  (stripped character sting)
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupA, ReadDataGroupB, ReadDataGroupC,
C-              ReadDataGroupD, ReadDataGroupE, ReadDataGroupF
C-
C-
C-  Created:    Aditya Maroo (code)
C-              Mark Velleux (code additions and comments)
C-
C-  Date:       05-AUG-2003
C-
C-  Revised:    Mark Velleux
C-
C-  Date:       11-FEB-2005
C-
C-  Revisions:  Renamed offset position counter from "j" to
C-              "offset" to eliminate shadow declaration of
C-              global variable "j".
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C----------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

char *StripString(char *s)
{
	//s is the incoming character string passed by its pointer *s

	//local variable declaration/definitions
	int
		offset;	//offset position in character string array

	//size_t is a data type for holding sizes returned by the strlen() function
	size_t
		len;	//length of the character string (after leading spaces removed)

	//initialize local variables
	offset = 1;	//starting value of the address offset is 1 (to skip the \0 at the end of all strings)

	//strip leading spaces from string...
	//
	//loop over string contents element by element until a non-blank
	//character is found or the end of the string is reached.
	//
	//loop over string contents while the character is blank or a tab and the string is not terminated
	while((*s == ' ' || *s == '\t') && (*s != '\0'))
	{
		//increment string address pointer (to skip blank character)
		s = s + 1;
	}

	//determine string length
	len = strlen(s);

	//check if the character one from the end of the string is a carriage return
	//(remember: the end of the string will always be '\0')
	//
	//s is the starting addres of the string (after removal of leading blanks) so
	//(s+len-1) is the position of the second to last character of the string...
	//
	//check if the second to last character of the string is a carriage return
	if(*(s + len - 1) == '\n')
	{
		len = len - 1;	//decrement the string length
	}

	//strip trailing blanks from string
	//
	//find the first trailing blank and overwrite it with a \0 to terminate
	//the string...
	//
	//loop over string while the character is blank or a tab and is not terminated
	while((*(s + len - offset) == ' ' || *(s + len - offset) == '\t') && (*s != '\n'))
	{
		//increment the address offset (to skip blank characters, count blanks)
		offset = offset + 1;
	}

	//terminate string: write '\0' in position of first trailing blank
	*(s + len - offset + 1) = '\0';

	//return string stripped of leading and trailing blanks and carriage return
	return(s);

//End of function
}
