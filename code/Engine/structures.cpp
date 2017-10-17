#include "engine.hpp"

//
// ARRAYS
//
#if ENGINE_ENABLE_ARRAYS
/*template <typename T> void readArray(Array<T> &a, FILE *fp){
    fread(&a.size, sizeof(unsigned int), 1, fp);
    a.data = (T *)malloc(a.size*sizeof(T));
    fread(a.data, sizeof(T), a.size, fp);
}
template <typename T> void readArray(Array<T> *a, FILE *fp){
    fread(&a->size, sizeof(unsigned int), 1, fp);
    a->data = (T *)malloc(a->size*sizeof(T));
    fread(a->data, sizeof(T), a->size, fp);
}
template <typename T> void writeArray(Array<T> &a, FILE *fp){
    fwrite(&a.size, sizeof(unsigned int), 1, fp);
    fwrite(a.data, sizeof(T), a.size, fp);
}
template <typename T> void writeArray(Array<T> *a, FILE *fp){
    fwrite(&a->size, sizeof(unsigned int), 1, fp);
    fwrite(a->data, sizeof(T), a->size, fp);
}
template <typename T> void free(Array<T> &a){
    if(a.size > 0 && a.data != nullptr)
        free(a.data);
    a.size = 0;
}*/
#endif


//
// STRINGS
//
#if ENGINE_ENABLE_STRINGS

void _strcpy(char *dst, const char *src){
int i;
for(i=0; src[i]!='\0'; i++){
    dst[i] = src[i];
}
dst[i] = '\0';
}
int _strlen(const char *s){
    int i;
    for(i=0; s[i]!='\0'; i++);
    return i;
}

void String::operator+=(char c){
    if(initialized == 1 && reserved > 0 && text != nullptr){
        u32 oldLength = length;
        length++;
        if(reserved < length){
            reserved = length;
            char *oldText = text;
            text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
            memcpy(text, oldText, oldLength*sizeof(char));
            allocator->deallocate(allocator, oldText);
        }
        text[length-1] = '\0';
        text[length-2] = c;
        initialized = true;
    }else{
        reserved = 20;
        length = 2;
        text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
        text[1] = '\0';
        text[0] = c;
        initialized = true;
    }
}
void String::operator+=(const char *s){
    unsigned long s_length = _strlen(s)+1;
    if(initialized == 1 && reserved > 0 && text != nullptr){
        u32 oldLength = length;
        length += s_length-1;
        if(reserved < length){
            reserved = length;
            char *oldText = text;
            text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
            memcpy(text, oldText, oldLength*sizeof(char));
            allocator->deallocate(allocator, oldText);
        }
        _strcpy(text+oldLength-1, s);
        initialized = true;
    }else{
        *this = s;
    }
}
void String::operator+=(String s){
    if(initialized == 1 && reserved > 0 && text != nullptr){
        u32 oldLength = length;
        length += s.length-1;
        if(reserved < length){
            reserved = length;
            char *oldText = text;
            text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
            memcpy(text, oldText, oldLength*sizeof(char));
            allocator->deallocate(allocator, oldText);
        }
        
        _strcpy(text+oldLength-1, s.text);
        initialized = true;
    }else{
        *this = s;
    }
}

bool String::operator==(String s){
    if(s.length != length)
        return false;
    for(int i=0; i<length; i++)
        if(s.text[i] != text[i])
            return false;
    return true;
}

bool String::operator==(const char *s){
    if(_strlen(s)+1 != length){
        return false;
    }
    for(int i=0; i<length; i++)
        if(s[i] != text[i])
            return false;
    return true;
}

bool String::operator!=(String s){
    return !(*this == s);
}

bool String::operator!=(const char *s){
    return !(*this == s);
}

String::String(){
    text = nullptr;
    length = 0;
    reserved = 0;
    initialized = true;
}

String::String(const char *s){
    length = _strlen(s)+1;
    reserved = length;
    text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    _strcpy(text, s);
    initialized = true;
    return;
}

String::String(u32 toReserve){
    reserved = toReserve;
    text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    text[0] = '\0';
    length = 0;
    initialized = true;
    return;
}

String::String(Allocator &alloc){
    text = nullptr;
    length = 0;
    reserved = 0;
    initialized = true;
    allocator = &alloc;
}

String::String(const char *s, Allocator &alloc){
    allocator = &alloc;
    length = _strlen(s)+1;
    reserved = length;
    text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    _strcpy(text, s);
    initialized = true;
    return;
}

String::String(u32 toReserve, Allocator &alloc){
    allocator = &alloc;
    reserved = toReserve;
    text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    text[0] = '\0';
    length = 0;
    initialized = true;
    return;
}

void String::reserve(u32 toReserve){
    if(initialized == 1 && reserved > 0 && text != nullptr){
        allocator->deallocate(allocator, text);
    }
    reserved = toReserve;
    text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    text[0] = '\0';
    length = 0;
    initialized = true;
    return;
}

void String::operator=(const char *s){
    if(initialized == 1 && reserved > 0 && text != nullptr){
        length = _strlen(s) + 1;
        if(reserved < length){
            reserved = length;
            allocator->deallocate(allocator, text);
            text = (char *)allocator->allocate(allocator, reserved * sizeof(char));
        }
    }else{
        length = _strlen(s)+1;
        reserved = length;
        text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    }
    _strcpy(text, s);
    initialized = true;
}

void String::operator=(String s){
    // This may cause segmentation faults
    if(s.length == 0){
        if(initialized == 1 && reserved > 0 && text != nullptr)
            allocator->deallocate(allocator, (char *)text);
        text = nullptr;
        length = 0;
        reserved = 0;
        initialized = false;
        return;
    }
    length = s.length;
    if(initialized == 1 && reserved > 0 && text != nullptr){
        if(reserved < s.length || allocator != s.allocator){
            allocator->deallocate(allocator, text);
            allocator = s.allocator;
            reserved = length;
            text = (char *)allocator->allocate(allocator, reserved * sizeof(char));
        }
    }else{
        allocator = s.allocator;
        reserved = length;
        text = (char *)allocator->allocate(allocator, reserved*sizeof(char));
    }
    _strcpy(text, s.text);
    initialized = true;
}

void free(String &s){
    s.initialized = false;
    s.length = 0;
    s.reserved = 0;
    s.allocator->deallocate(s.allocator, s.text);
}

const Angle zeroAngle = Angle(1.f, 0.f);

#endif
