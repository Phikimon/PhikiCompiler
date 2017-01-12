# -*- coding: utf-8 -*-
import sys

class Translit:
    kirilica = ['Щ','щ','Е','е','Ё','ё','Ж','ж','Ч','ч','Ш','ш','Ю','ю','Я','я',
        'А','Б','В','Г','Д','З','И','Й','К','Л','М','Н','О','П','Р','С','Т',
        'У','Ф','Х','Ц','Ъ','Ы','Ь','Э','а','б','в','г','д','з','и','й','к',
        'л','м','н','о','п','р','с','т','у','ф','х','ц','ъ','ы','ь','э','В',
        'в','Кс','кс','К','к']
    translit = ['SCH','sch','E','e','YO','yo','ZH','zh','CH','ch','SH','sh',
        'YU','yu','YA','ya','A','B','V','G','D','Z','I','I','K','L','M','N',
        'O','P','R','S','T','U','F','H','C','','J','','E','a','b','v','g',
        'd','z','i','i','k','l','m','n','o','p','r','s','t','u','f','h','c',
        '','j','','e','W','w','X','x','Q','q']
 
    def toTranslit(self,text):
        newText = text;
        for key in xrange(len(self.kirilica)) :
            newText = newText.replace(self.kirilica[key],self.translit[key])
        return newText
       
f = open(sys.argv[1])
transtiter = Translit()


while True:
    line = f.readline()
    if len(line) == 0: # Нулевая длина обозначает конец файла (EOF)
        break
    print transtiter.toTranslit(line)

f.close()
