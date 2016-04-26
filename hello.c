#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yad3service.h"
#include "mtm_ex2.h"

void printYad3Result (Yad3Result result) {
	   switch(result) {
		case YAD3_SUCCESS : printf("YAD3_SUCCESS\n" ); break;
		case YAD3_OUT_OF_MEMORY : printf("YAD3_OUT_OF_MEMORY\n" ); break;
		case YAD3_INVALID_PARAMETERS : printf("YAD3_INVALID_PARAMETERS\n" ); break;
		case YAD3_EMAIL_ALREADY_EXISTS : printf("YAD3_EMAIL_ALREADY_EXISTS\n" ); break;
		case YAD3_EMAIL_DOES_NOT_EXIST : printf("YAD3_EMAIL_DOES_NOT_EXIST\n" ); break;
		case YAD3_EMAIL_WRONG_ACCOUNT_TYPE : printf("YAD3_EMAIL_WRONG_ACCOUNT_TYPE\n" ); break;
		case YAD3_ALREADY_REQUESTED : printf("YAD3_ALREADY_REQUESTED\n" ); break;
		case YAD3_NOT_REQUESTED : printf("YAD3_NOT_REQUESTED\n" ); break;
		case YAD3_APARTMENT_SERVICE_ALREADY_EXISTS : printf("YAD3_APARTMENT_SERVICE_ALREADY_EXISTS\n" ); break;
		case YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST : printf("YAD3_APARTMENT_SERVICE_DOES_NOT_EXIST\n" ); break;
		case YAD3_APARTMENT_SERVICE_FULL : printf("YAD3_APARTMENT_SERVICE_FULL\n" ); break;
		case YAD3_APARTMENT_ALREADY_EXISTS : printf("YAD3_APARTMENT_ALREADY_EXISTS\n" ); break;
		case YAD3_APARTMENT_DOES_NOT_EXIST : printf("YAD3_APARTMENT_DOES_NOT_EXIST\n" ); break;
		case YAD3_PURCHASE_WRONG_PROPERTIES : printf("YAD3_PURCHASE_WRONG_PROPERTIES\n" ); break;
		case YAD3_REQUEST_WRONG_PROPERTIES : printf("YAD3_REQUEST_WRONG_PROPERTIES\n" ); break;
		case YAD3_REQUEST_ILLOGICAL_PRICE : printf("YAD3_REQUEST_ILLOGICAL_PRICE\n" ); break;
	    default : printf("INAVLID ERROR CODE - PLEASE REVIEW YOUR CODE\n" );
	   }
}

int main () {

	Yad3Service service = yad3ServiceCreate();

	yad3RealtorAdd(service, "dave@gmail", "Davidson", 12);
	yad3RealtorAdd(service, "jessica@gmail", "Rabbits inc.", 11);
	yad3RealtorAdd(service, "ned_stark@gmail", "Winter is coming", 14);

//	SquareType s0[] = { WALL, EMPTY, WALL };
//	SquareType s1[] = { WALL, EMPTY, EMPTY };
//	SquareType s2[] = { WALL, WALL, WALL };
//	SquareType s3[] = { EMPTY, EMPTY, EMPTY };
//	SquareType* grid1[4] = { s0, s1, s2, s3 };	//Area = 6
//	SquareType* grid2[4] = { s0, s1, s1, s1 };	//Area = 7
//	SquareType* grid3[4] = { s3, s3, s1, s3 };	//Area = 10
//	SquareType* grid4[4] = { s3, s3, s3, s3 };	//Area = 12
//	SquareType* grid5[4] = { s0, s0, s2, s0 };	//Area = 3
//	Apartment apartment1 = apartmentCreate(grid1, 4, 3, 150000);//Area = 6, Price = 150000
//	Apartment apartment2 = apartmentCreate(grid2, 4, 3, 690000);//Area = 7, Price = 690000
//	Apartment apartment3 = apartmentCreate(grid3, 4, 3, 100000);//Area = 10, Price = 100000
//	Apartment apartment4 = apartmentCreate(grid4, 4, 3, 800080);//Area = 12, Price = 800080
//	Apartment apartment5 = apartmentCreate(grid5, 4, 3, 300000);//Area = 3, Price = 300000

	char* mat1 = "wewweewwweee";	//Area = 6
	char* mat2 = "eewweeweewee";	//Area = 7
	char* mat3 = "eeeeeeweweee";	//Area = 10
	char* mat4 = "eeeeeeeeeeee";	//Area = 12
	char* mat5 = "wewwewwwwwew";	//Area = 3

	yad3RealtorAddApartmentService(service, "dave@gmail", "ap1", 3);
	yad3RealtorAddApartmentService(service, "dave@gmail", "ap2", 5);

	yad3RealtorAddApartment(service, "dave@gmail", "ap1", 10132, 150000, 4, 3, mat1);
	yad3RealtorAddApartment(service, "dave@gmail", "ap1", 10133, 690000, 4, 3, mat2);
	yad3RealtorAddApartment(service, "dave@gmail", "ap2", 10132, 300000, 4, 3, mat5);
	yad3RealtorAddApartment(service, "dave@gmail", "ap2", 10133, 690000, 4, 3, mat2);
//	yad3RealtorAddApartment(service, "dave@gmail", "ap2", 10132, 150000, 4, 3, mat1);


	yad3RealtorAddApartmentService(service, "jessica@gmail", "ap1", 3);
	yad3RealtorAddApartmentService(service, "jessica@gmail", "ap2", 5);
	yad3RealtorAddApartmentService(service, "jessica@gmail", "ap3", 3);

	yad3RealtorAddApartment(service, "jessica@gmail", "ap1", 10132, 100000, 4, 3, mat3);
	yad3RealtorAddApartment(service, "jessica@gmail", "ap1", 10133, 690000, 4, 3, mat2);
	yad3RealtorAddApartment(service, "jessica@gmail", "ap2", 10132, 800080, 4, 3, mat4);
	yad3RealtorAddApartment(service, "jessica@gmail", "ap2", 10133, 150000, 4, 3, mat1);
	yad3RealtorAddApartment(service, "jessica@gmail", "ap3", 10132, 150000, 4, 3, mat1);
	yad3RealtorAddApartment(service, "jessica@gmail", "ap3", 10133, 690000, 4, 3, mat2);


	yad3RealtorAddApartmentService(service, "ned_stark@gmail", "ap1", 3);
	yad3RealtorAddApartmentService(service, "ned_stark@gmail", "ap2", 5);
	yad3RealtorAddApartmentService(service, "ned_stark@gmail", "ap3", 3);

	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap1", 10132, 300000, 4, 3, mat5);
	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap1", 10133, 150000, 4, 3, mat1);
	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap2", 10132, 100000, 4, 3, mat3);
	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap2", 10133, 150000, 4, 3, mat1);
	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap3", 10132, 800080, 4, 3, mat4);
	yad3RealtorAddApartment(service, "ned_stark@gmail", "ap3", 10133, 690000, 4, 3, mat2);


	yad3SignificantRealtors(service, 4);

	yad3RealtorRemove(service, "dave@gmail");
	yad3RealtorRemove(service, "jessica@gmail");
	yad3RealtorRemove(service, "ned_stark@gmail");



	yad3ServiceDestroy(service);
	return 0;
}

