#ifndef _D3DUTILITY_H_
#define _D3DUTILITY_H_

template<typename T>
void Release(T t) 
{
    if (t)
    {
        t->Release();
        t = 0;
    }
}

template<typename T>
void Delete(T t)
{
    if (t)
    {
        delete t;
        t = 0;
    }
}
#endif
