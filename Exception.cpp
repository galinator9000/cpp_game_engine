#include "Exception.h"

Exception::Exception(LPCTSTR errorText){
	MessageBox(
		NULL,
		errorText,
		NULL,
		(MB_OK | MB_ICONWARNING)
	);
}