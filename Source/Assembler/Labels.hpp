#ifndef LABELS_HPP_INCLUDED
#define LABELS_HPP_INCLUDED

enum { MAX_LBL_QT      = 64 };
enum { MAX_LBL_LEN     = 64 };

class AsmLabels
{
    public:
        AsmLabels () {};
        ~AsmLabels() {};

        void         dump();
        int          name2Addr(const char* name);
        int          num2Addr(unsigned num);
        const char*  num2Name(unsigned num);
        void         appendLabel(const char* name, unsigned addr);
        const char*  stdLabel();
        int          getNum();
    private:
        const char STD_LABEL[sizeof("PhilIsCoolGuy")] = "PhilIsCoolGuy";
        int        labelNum_                          = 0 ;
        char       names_[MAX_LBL_QT][MAX_LBL_LEN]    = {};
        int        addresses_[MAX_LBL_QT]             = {};
};

#endif
