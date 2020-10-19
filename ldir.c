/* ldir.c -- программа, выводящая на экран содержимое директории,
 * переданной в качестве параметра командной строки. В случае
 * отсутствия параметров командной строки печатется содержимое
 * текущей директории. Вывод программы аналогичен выводу
 * команды интерпретатора ls -al */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<grp.h>
#include<pwd.h>
#define MAX_SZ 256	// Максимальный размер буфера для хранения времени
			// модификации файла

int main (int argc, char * argv[])
{
	struct stat fattr;	// структура для хранения атрибутов файла
	DIR * dptr;		// указатель на структуру директории
	struct dirent * recptr;	// указатель на запись в директории
	struct passwd * pswd;	// указатель на структуру, возвращаемую
				// вызовом getpwuid (нужен для получения
				// имени пользователя в строковом виде)
	struct group * grp;	// указатель на структуру, возвращаемую
				// вызовом getgrgid (нужен для получения
				// имени группы в виде строки)
	char buf [MAX_SZ];	// буфер для чтения времени модификации
				// файла в строковом виде

	/* Проверяем корректность передачи параметров программе */
	if (argc > 2)
	{
		printf("Error! Usage: %s [directory's pathname]\n", argv[0]);
		exit(-1);
	}
	else if (argc == 1)
		/* Если в параметрах путь до директории не передан
		 * , открываем текущую */
		dptr = opendir(".");
	else
		/* В противном случае открываем директорию,
		 * указанную в параметре */
		dptr = opendir(argv[1]);

	/* Последовательно читаем записи в директории */
	while ((recptr = readdir(dptr)) != NULL)
	{
		/* считываем в структуру fattr атрибуты файла */
                if (lstat(recptr->d_name, &fattr) < 0)
                        printf("Can't get file attributes for file.");
		else
		{
			/* Организуем вывод на stdout типа файла */
			switch(recptr->d_type)
			{
				case DT_BLK:
					printf("b");
					break;
				case DT_CHR:
                                	printf("c");
                                	break;
				case DT_DIR:
                                	printf("d");
                                	break;
				case DT_FIFO:
                                	printf("p");
                                	break;
				case DT_LNK:
                                	printf("l");
                                	break;
				case DT_REG:
                                	printf("-");
                                	break;
				case DT_SOCK:
                                	printf("s");
                                	break;
			}
			/* Серия ветвлений для отображения на stdout
		 	* прав доступа к файлу. Младшие 9 бит поля
		 	* st_mode структуры fattr содержат права 
		 	* доступа к файлу. */
			if (fattr.st_mode & 0b100000000)
				printf("r");
			else
				printf("-");
			if (fattr.st_mode & 0b10000000)
				printf("w");
			else
				printf("-");
			if (fattr.st_mode & 0b1000000)
				printf("x");
			else
				printf("-");
			if (fattr.st_mode & 0b100000)
				printf("r");
			else
				printf("-");
			if (fattr.st_mode & 0b10000)
				printf("w");
			else
				printf("-");
			if (fattr.st_mode & 0b1000)
				printf("x");
			else
				printf("-");
			if (fattr.st_mode & 0b100)
				printf("r");
			else
				printf("-");
			if (fattr.st_mode & 0b10)
				printf("w");
			else
				printf("-");
			if (fattr.st_mode & 0b1)
				printf("x");
			else
				printf("-");
		
			/* Выводим число жестких ссылок на файл */
			printf(" %ld ", (long) fattr.st_nlink);
			/* Заполняем структуру passwd данными.
		 	* Необходимо для получения username
		 	* по uid */
			pswd = getpwuid(fattr.st_uid);
			/* Отображаем username владельца */
			printf("%s ", pswd->pw_name);
			/* Заполняем структуру group.
		 	* Необходимо для получения названия группы
		 	* по gid */
			grp = getgrgid(fattr.st_gid);
			/* Выводим название группы, к которой
		 	* принадлежит владелец */
			printf("%s ", grp->gr_name);
			/* Печатаем размер файла */
			printf("%10ld ", (long) fattr.st_size);
			/* считываем в buf время модификации файла в строковом виде */
			ctime_r(&fattr.st_mtime, buf);
			/* Заменяем '\n' на '\0' в buf */
			buf[strlen(buf) - 1] = '\0';
			/* Печатаем время модификации файла */
			printf("%s ", buf);
			/* Печатаем имя файла */
			printf("%s ", recptr->d_name);
			/* Если файл имеет тип "ссылка", требуется добавить 
		 	* pathname, на который он ссылается */
			if (recptr->d_type == DT_LNK)
			{
				/* Поскольку функция readlink формирует
			 	* не нультерминированную строку,
			 	* сначала забиваем буфер нулями */
				for (int i = 0; i < MAX_SZ; i++)
					buf[i] = 0;
				/* А затем, читаем содержимое "ссылки"
			 	* в buf*/
				if (readlink(recptr->d_name, buf, 256) < 0)
					printf("-> Error while reading soft link.");
				else
					/* Дописываем содержимое "ссылки" в stdout
			 		* так, как это делает команда ls -la*/
					printf("-> %s", buf);
			}
		}
		printf("\n");
	}


	return 0;
}
