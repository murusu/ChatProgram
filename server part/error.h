#include <winsock.h>
#include <stdio.h>

class ErrorObject
{
	private:

	public:
		char szWSAErrorMessage[128];

		void vGetWSAErrorMessage(int err);
};