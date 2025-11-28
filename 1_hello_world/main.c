#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main()
{
    char greeting[MAX_STRING];
    int comm_sz; // Number of processes
    int my_rank; // My process rank

    // argc, argv를 받을 수 있음
    MPI_Init(NULL, NULL);

    // communicator == collection of processes that can communicate with each other
    // MPI_COMM_WORLD == communicator

    // return the number of processes in the communicator
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    // return the rank of the calling process in the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0)
    {
        snprintf(greeting, MAX_STRING, "Hello from process %d of %d!", my_rank, comm_sz);

        // void* msg_buf_p, int msg_size, msg_type, int dest, int tag, comm
        // tag: message identifier
        MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        printf("Hello from process %d of %d!\n", my_rank, comm_sz);
        for (int q = 1; q < comm_sz; q++)
        {
            // void* msg_buf_p, int msg_size, msg_type, int source, int tag, comm, status
            // out, in, in, in, in, in, out
            // 받는 source rank를 지정할 수 있음
            // 이는 먼저 끝난 rank들은 다른 rank들이 보낸 메시지를 받을때 까지 blocking 될 수 있음을 의미
            // MPI_ANY_SOURCE를 사용하면 어떤 rank가 보내든지 상관없이 메시지를 받을 수 있음
            // MPI_ANY_TAG도 동일하게 동작
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // 사실 sender와 tag, 크기를 모르고도 받을 수 있음
            // 이 정보는 status 객체를 통해 알 수 있음
            // status.MPI_SOURCE, status.MPI_TAG, MPI_Get_count(&status, MPI_CHAR, &count{output}) 등으로 접근 가능
            printf("recv: %s\n", greeting);
        }
    }

    // 꼭 INIT과 FINALIZE가 main에 있을 필요는 없음
    MPI_Finalize();
    return 0;
}

// MPI send는 buffered 혹은 blocking 방식으로 동작(구현 의존) (큰 메시지의 경우 buffered 방식으로 동작)
// - return의 기준은 둘 다 buffer에 복사가 완료되었는지 여부
// - 즉 둘 다 전송 완료를 보장하지 않음
// MPI send는 overtaking을 방지함
// - 즉 같은 process에서 같은 destination으로 보내는 메시지들은 순서가 보장됨
// - 단 다른 process들에서 받는 메시지들은 순서가 보장되지 않음
// MPI recv는 blocking 방식으로 동작