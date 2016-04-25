#include <stdio.h>
#include <stdlib.h>
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
	printf("Printing all Realtors:\n");
	Yad3Service service = yad3ServiceCreate();

	Yad3Result res;

	res = yad3AddRealtor(service, "brizer2@gmail.com", "RealState", 100);
	printYad3Result(res);
	res = yad3AddRealtor(service, "brizer2@gmail.com", "Remax", 100);
	printYad3Result(res);
	res = yad3AddRealtor(service, "brizer32@gmail.com", "Remax", 100);
	printYad3Result(res);
	res = yad3AddApartmentService(service, "brizer2@gmail.com", "luah Zafon", 500);
	printYad3Result(res);
	res = yad3AddApartmentService(service, "brizer2@gmail.com", "luah Darom", 500);
	printYad3Result(res);
	res = yad3AddApartmentService(service, "brizer3@gmail.com", "Only for rich", 500);
	printYad3Result(res);
	res = yad3AddApartmentService(service, "brizer32@gmail.com", "Only for rich", 500);
	printYad3Result(res);
	res = yad3AddApartmentService(service, "brizer32@gmail.com", "Only for rich", 500);
	printYad3Result(res);
	res = yad3RemoveApartmentService(service, "brizer2@gmail.com", "Only for rich");
	printYad3Result(res);
	res = yad3RemoveApartmentService(service, "brizer32@gmail.com", "Only for rich");
	printYad3Result(res);
	//mtmPrintRealtor(stdout,"brizer2@gmail.com", "RealState");

	yad3DeleteRealtor(service, "brizer2@gmail.com");
	yad3DeleteRealtor(service, "brizer32@gmail.com");

	yad3ServiceDestroy(service);
	return 0;
}

