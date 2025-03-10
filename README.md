# COMP2432 Operating Systems Group Project

> **Scenario:** 
> 
> PolyU University is an educational institution with a diverse population of students, faculty, and staff
who require convenient access to parking facilities on campus. PolyU finds that there is a problem with its current
parking management system designed 10 years ago. It is not flexible enough to make good utilization of the
parking spaces and to satisfy most requests from its members. At PolyU, there are just 10 parking spaces available.
Additionally, six essential facilities are offered in pairs to improve the overall user experience: (battery + cable),
(lockers + umbrella), and (inflation service + valet parking). When a user reserves a battery, they automatically
receive a cable as well. If a user reserves both a battery and a locker, they will be provided with a battery, cable,
locker, and umbrella. 
> 
> Currently, members of PolyU may simply make a reservation for a parking space or reserve only the parking
facilities (essentials). For example, Member_A makes a reservation for parking space for their smart vehicle with
a battery and charging cable. If they are both (parking + essentials) available, the reservation request is accepted.
Otherwise, the reservation request is simply rejected. For instance, all the electric vehicle charging cable might
have been booked by other members, so the request is rejected. In other words, the current system does not provide
any alternative plan to satisfy or reschedule the reservation when some of the requested items or locations are not
available for the requested time slot. Knowing that you have learned different scheduling methods from Operating
Systems, the CFSO in PolyU is offering you a part-time job that you could use to satisfy your WIE. Would you
mind helping CFSO at PolyU to revise its parking management system to achieve better utilization with an
improved scheduling/rescheduling method? The goal is to improve the reservation situation so as to increase the
PolyU’s revenue from renting these spaces to its members.

## Project Requirements
In this project, you will be given an opportunity to apply the theory of process scheduling you have learnt from  COMP2432 to a daily-life scenario and produce the **Smart Parking Management System (SPMS)**. The project consists of the following parts:
1. **Develop a program that allows users to add details of a booking (name, parking slot number, date,  time, duration, and/or calless etc.) to the schedule.** Besides the standard line by line input approach, SPMS should be able to read in batch files which containing the booking requests, i.e. one or more than one booking requests are stored in such batch files. Note that the one who initiates for the booking is called the “client” or the “requester” and those others involved in the booking are called “staff”. This part of the program module is referred to as the **Input Module**.
2. **Extend the program developed in Part I with a scheduler to generate timetables for all bookings (e.g. available parking spots, and members booking/reservation records).** The scheduler will implement several scheduling algorithms similar to those covered in lectures. This is the called the **Scheduling Kernel**, within the **Scheduling Module**.
3. Augment the program with the facilities to print booking schedule for available parking slots and related additional facilities and equipment in Part II. Those rejected bookings should all be included. This constitutes the **Output Module**.