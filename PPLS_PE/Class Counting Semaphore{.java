Class Counting Semaphore{
    int count = n;
    void synchronized P(){
        while(count <= 0){
            wait();
        }
        count--;
    }
    void synchronized V(){
        count++;
        notify();
    }
}

SELECT * 
FROM station 
JOIN trip ON trip.start_station_id = station.station_id
JOIN weather ON weather.zip_cod = station.zip_code
WHERE weather.date BETWEEN "2013-01-01" AND "2013-12-31"
AND trip.bike_id = 200 
AND trip.duration < 100
GROUP BY station.city, trip.start_station_id
ORDER BY station.city DESC


// PPLS 2016 2.(b)
shared int lock = 1; 
co [i = 1 to n]{
    do{
        while(lock <= 0);
    }while(DEC(lock));
    // while(lock <= 0 || DEC(lock));
    // Critical Section
    lock = 1;
}

// PPLS 2015 1.(c)
pthread_mutex_t lock;
pthread_cond_t cv;
shared int S = 0;
// < await S > 0 S--; >
P(){
    lock(&lock);
    while(S <= 0){
        wait(&cv, &lock);
    }
    S--;
}
// S++;
V(){
    S++;
    signal(&cv, &lock) // Key
    unlock(&lock);
}

// PPLS 2015 1.(d)
pthread_mutex_t lock;
pthread_cond_t cv;
shared int amount = 0;

void payIn(int n){
    lock(&lock);
    amount += n;
    signal(&cv, &lock);
    unlock(&lock);
}

void withdraw(int n){
    lock(&lock);
    while(amount < n){
        wait(&cv, &lock);
    }
    amount -= n;
    unlock(&lock);
}

// PPLS 2015 3.(b)
void sumToP0(MPI_Comm c){
    MPI_Comm nextComm;
    int p, r;
    MPI_Comm_size(c, &p);
    MPI_Comm_rank(c, &r);
    if(p == 1) return;
    else if(p > 1){
        MPI_Comm_split(c, r < (p / 2), 0, &nextComm);
        sumToP0(nextComm);   
        
        if(r == 0){
            int receivedVal;
            MPI_Recv(&receivedVal, 1, MPI_INT, MPI_ANY_SOURCE, 0, c, MPI_STATUS_IGNORE);
            total = myval + receivedVal;
        }else if(r == p/2) MPI_Send(&myval, 1, MPI_INT, 0, 0, c); // Send to root
    }
}