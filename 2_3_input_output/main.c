#include <stdio.h>
#include <mpi.h>

// 대부분의 구현에서
// stdin은 rank 0에서만 허용되도록 구현된다.
// stdout과 stderr는 모든 rank에서 허용된다.

// 아래의 Get_data 함수는 rank 0에서만 사용자로부터 입력을 받고
// 다른 rank들에게는 MPI_Send와 MPI_Recv를 사용하여 데이터를 전송한다.
// 이는 병렬 프로그램에서 표준 입력을 처리하는 일반적인 방법이다.

double Trap(double local_a, double local_b, int local_n, double h);
void Get_data(int my_rank, int comm_sz, double *a_p, double *b_p, int *n_p)
{
    if (my_rank == 0)
    {
        printf("Enter a, b, and n\n");
        scanf("%lf %lf %d", a_p, b_p, n_p);
        for (int dest = 1; dest < comm_sz; dest++)
        {
            MPI_Send(a_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(b_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(n_p, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(n_p, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

int main()
{
    int my_rank, comm_sz, n = 1024, local_n;
    double a = 0.0, b = 3.0, h, local_a, local_b;
    double local_int, total_int;
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Get_data(my_rank, comm_sz, &a, &b, &n);

    h = (b - a) / n;       // step size
    local_n = n / comm_sz; // number of subintervals for each process

    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    local_int = Trap(local_a, local_b, local_n, h);

    if (my_rank != 0)
    {
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        total_int = local_int;
        for (source = 1; source < comm_sz; source++)
        {
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
        printf("With n = %d trapezoids, our estimate of the integral from %.2f to %.2f = %.15f\n", n, a, b, total_int);
    }

    if (my_rank == 0)
    {
        printf("With n = %d trapezoids, our estimate\n", n);
        printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
    }

    MPI_Finalize();
    return 0;
}

int f(double x)
{
    return x * x;
}

double Trap(double left_endpt, double right_endpt, int local_n, double h)
{
    double estimate, x;
    int i;
    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= local_n - 1; i++)
    {
        x = left_endpt + i * h;
        estimate += f(x);
    }
    estimate = estimate * h;
    return estimate;
}