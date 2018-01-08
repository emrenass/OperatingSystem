//
// Created by emrenas on 11/23/17.
//

#ifndef STUDENT_H
#define STUDENT_H
struct student{
    int sid;
    char firstname[64];
    char lastname[64];
    float cgpa;
};

struct producerParam{
    struct student* arr;
    int id;
    int size;
};

struct consumerParam{
    int n;
    int id;
    int sumOfStudents;
    int *sizeOfProducer;
};
#endif

