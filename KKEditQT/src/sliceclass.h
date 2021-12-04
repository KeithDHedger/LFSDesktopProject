/******************************************************
*
*     ©keithhedger Thu  5 Jun 14:36:43 BST 2014
*     kdhedger68713@gmail.com
*
*     sliceclass.h
*
******************************************************/

#ifndef _SLICECLASS_
#define _SLICECLASS_

#define NUMHTMLENTRIES 252
enum	errorCodes {NOERROR=0,NOMATCH,NULLSRC,MULTMATCH};

/**
 *
 *
 * \brief String slicing
 *
 * General purpose string slicing class.
 *
 * \note Returns a pointer to a static buffer DO NOT FREE!
 * \note Check error with getResult(), return value can be used as source for next call. 
 *
 *
 */
 
class StringSlice
{
	public:
		StringSlice();
		~StringSlice();

		char* 	sliceBetween(char* src,char* startstr,char* endstr);
		char* 	sliceInclude(char* src,char* startstr,char* endstr);
		char* 	sliceLen(char* src,int start,int len);
		char* 	slice(char* src,int start,int end);
		char* 	sliceStrLen(char* src,char* startstr,int len);
		char* 	deleteSlice(char* src,char* delstr);
		void	setCaseless(bool caseval);
		void	copyToBuffer(const char* str);
		int		getResult(void);
		void	setReturnDupString(bool want);
		char*	decodeHtml(char* src);
		char*	randomName(int len);
		bool	getDuplicate(void);
	private:
		void	checkBufferLen(char* str);
		void	checkBufferLen(int size);
		char*	returnData(char* str);
		char*	buffer;
		bool	caseless;
		int		error;
		bool	duplicate;
		unsigned int	bufferlen;
};

#endif

