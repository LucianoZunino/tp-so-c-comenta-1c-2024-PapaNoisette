#include "escuchar_kernel_cpu.h"

t_pcb* EXEC;

void escuchar_mensajes_kernel_dispatch(){
	t_buffer * buffer=crear_buffer();
    bool desconexion_kernel_dispatch = 0;

	while(!desconexion_kernel_dispatch){
		int cod_op = recibir_operacion(fd_kernel_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
			//printf("---recibir_operacion--->cod_op= %d\n",cod_op);

		switch(cod_op){
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_cpu, fd_kernel_dispatch, cod_op);

				break;
			case KERNEL_ENVIA_PROCESO:
				buffer = recibir_buffer_completo(fd_kernel_dispatch);
			    //EXEC=malloc(sizeof(t_pcb));
				EXEC = deserializar_pcb(buffer); 
				print_pcb(EXEC);
				flag_interrupt = false;
    			//sleep(2);

				while(ciclo_de_instruccion(EXEC) == 0){
					print_pcb(EXEC);
					sleep(1);
				};

 				break;
			case -1:
				log_error(logger_cpu, "El Kernel se desconecto de Dispatch. Terminando servidor.");
				desconexion_kernel_dispatch = 1;

				break;
			default:
				log_warning(logger_cpu, "Operacion desconocida de Kernel-Dispatch. ABORTANDO");
				desconexion_kernel_dispatch = 1;

				break;
		}
	}
	
	printf("FINALIZA HILO: escuchar_mensajes_kernel_dispatch= ");
}

void escuchar_mensajes_kernel_interrupt(){
    bool desconexion_kernel_interrupt = 0;
	printf("\n ESCUCHAR INTERRUPT DESDE CPU \n");
	while(!desconexion_kernel_interrupt){
		int cod_op = recibir_operacion(fd_kernel_interrupt); // recv() es bloqueante por ende no queda loopeando infinitamente
		printf("\n COP_OP EN INTERRUPT: %i \n", cod_op);
		switch(cod_op){
			t_buffer* buffer;
			t_pcb* pcb;
			t_paquete* paquete;
			t_buffer* buffer_a_enviar;
			op_code motivo;
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_cpu, fd_kernel_interrupt, cod_op);

				break;
			case ELIMINAR_PROCESO:
				printf("\nENRTO A ELIMINAR PROCESO CPU\n");
				//t_buffer* buffer = crear_buffer();
				buffer = recibir_buffer_completo(fd_kernel_interrupt);
				pcb = deserializar_pcb(buffer);
				motivo = extraer_int_del_buffer(buffer);
				printf("\nMOTIVO DE INTERRUPCION: %i\n", motivo);
				if(EXEC == NULL || pcb->pid != EXEC->pid){
					break;
				}
				flag_interrupt = true;
				printf("\nDESP DEL FLAG_INTERRUPT\n");
				sem_wait(&sem_interrupt);
				
				buffer_a_enviar = crear_buffer();
				paquete = crear_paquete(motivo, buffer_a_enviar);
				agregar_pcb(paquete, EXEC);
				enviar_paquete(paquete, fd_kernel_dispatch);
				eliminar_paquete(paquete);
				EXEC = NULL;
				printf("\nDESP DEL enviar_proceso_por_paquete\n");
				break;
			case FIN_DE_QUANTUM:
				printf("\nENTRO A FIN DE QUANTUM\n");
				//t_buffer* buffer = crear_buffer();
				buffer = recibir_buffer_completo(fd_kernel_interrupt);
				int pid = extraer_int_del_buffer(buffer);
				motivo = extraer_int_del_buffer(buffer);
				printf("\nMOTIVO DE INTERRUPCION: %i\n", motivo);
				if(EXEC == NULL || pid != EXEC->pid){
					break;
				}
				flag_interrupt = true;
				printf("\nDESP DEL FLAG_INTERRUPT\n");
				sem_wait(&sem_interrupt);
				
				buffer_a_enviar = crear_buffer();
				paquete = crear_paquete(motivo, buffer_a_enviar);
				agregar_pcb(paquete, EXEC);
				EXEC = NULL;
				enviar_paquete(paquete, fd_kernel_dispatch);
				eliminar_paquete(paquete);
				printf("\nDESP DEL enviar_proceso_por_paquete\n");
				break;
			case -1:
				log_error(logger_cpu, "El Kernel se desconecto de Interrupt. Terminando servidor.");
				desconexion_kernel_interrupt = 1;

				break;
			default:
				log_warning(logger_cpu, "Operacion desconocida de Kernel-Interrupt.");

				break;
		}
	}
}

int recibir_interrupcion(){
	t_buffer* buffer = crear_buffer();
	buffer = recibir_buffer_completo(fd_kernel_interrupt);
	op_code motivo = extraer_int_del_buffer(buffer);
	destruir_buffer(buffer);
	return motivo;
}