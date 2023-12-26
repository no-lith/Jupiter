//=============================================================================
// File: 		LinkedList.h
//
// Purpose:		Linked list class
//
//  Copyright (C) 2003 Touchdown Entertainment Inc.
//  All Rights Reserved.
//=============================================================================

#ifndef __LINKEDLIST_H_
#define __LINKEDLIST_H_

#include <stdlib.h>

template <class T> class LinkedMember
{
  public:
  LinkedMember()
  {
	  next = NULL;
  }
  ~LinkedMember(){}

  T data;
  LinkedMember* next;
};

template <class T> class LinkedList
{
private:
	LinkedMember<T> *data;
	int m_nSize;

public:
	LinkedList()
	{
		data = NULL;
		m_nSize = 0;
	}

	~LinkedList()
	{
		if(data != NULL)
		{
			FreeStack();
		}
	}

	int GetSize()
	{
		return m_nSize;
	}

	void FreeStack()
	{
		LinkedMember<T> *_data;
		_data = data;

		for(;_data;)
		{
			LinkedMember<T> *pNext;
			pNext = _data->next;
			delete(_data);
			_data = pNext;
		}
        data = NULL;
	}

	T& operator [] (unsigned int i)
	{
		LinkedMember<T> *pNext;
		pNext = data;

		if(NULL != data)
		for(unsigned int j = 0; j < m_nSize; j++)
		{
			if(j == i)
			return pNext->data;

			pNext = pNext->next;
		}

		return data->data;//0x00;
	}

    LinkedMember<T>* First()
    {
        // Head pointer
        return data;
    }

	void Append(T value)
	{
		LinkedMember<T> *stackmember;
		stackmember = new LinkedMember<T>;
		if( stackmember != NULL )
		{
			stackmember->data = value;
			stackmember->next = data;
			data = stackmember;
			m_nSize++;
		}
	}

	void Remove(LinkedMember<T>* pEntry)
	{

		LinkedMember<T> *pPrev = NULL;
		LinkedMember<T> *pThis;
		pThis = data;

        while(pThis)
        {
            //Remove it and fuse the list together
            if(pEntry == pThis)
            {
                //Were we the first element in the list?
                if(NULL == pPrev)
                {
                    data = pThis->next;
                }
                else
                {
                    pPrev->next = pThis->next;
                }

                if(pThis)
                {
                    delete pThis;
                    m_nSize--;

                    if(m_nSize == 0)
                    {
                        data = NULL;
                    }
                }

                // We're done, exit the loop.
                // Otherwise we'll crash when the list is next traversed.
                return;
            }

			pPrev = pThis;
			pThis = pThis->next;
        }

    }
};

#endif
