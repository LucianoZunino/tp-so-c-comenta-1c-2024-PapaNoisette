#include "escuchar_kernel_cpu.h"

t_pcb* EXEC;

void escuchar_mensajes_kernel_dispatch(){
	printf("Entro a  escuchar_mensajes_kernel_dispatch \n");
t_buffer * buffer;
    bool desconexion_kernel_dispatch = 0;
	while(!desconexion_kernel_dispatch){

		int cod_op = recibir_operacion(fd_kernel_dispatch); // recv() es bloqueante por ende no queda loopeando infinitamente
			printf("---recibir_operacion--->cod_op= %d\n",cod_op);
				//aceptar_handshake(logger_cpu, fd_kernel_dispatch, cod_op);

		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			//	break;
						
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_cpu, fd_kernel_dispatch, cod_op);
				printf(" case aceptar_handshake \n");

				break;

			case KERNEL_ENVIA_PROCESO:
				printf("CASE: KERNEL_ENVIA_PROCESO\n");
				buffer=recibir_buffer_completo(fd_kernel_dispatch);
				//t_proceso_cpu *proceso=malloc(sizeof(t_proceso_cpu));

                //proceso->pid = extraer_int_del_buffer(buffer);
				//printf("------>proceso->pid= %d\n",proceso->pid);

                //proceso->program_counter = extraer_int_del_buffer(buffer);
				//printf("------>proceso->program_counter= %d\n",proceso->program_counter);

                //proceso->registros_cpu = extraer_datos_del_buffer(buffer);
				//print_registros(proceso->registros_cpu);

				/*	while (ciclo_de_instruccion(proceso) == 0)
					{
						sleep(1); // TODO: Quitar sleep cuando este implementado el tiempo de retardo en la memoria
					}*/
				EXEC = deserializar_pcb(buffer); //PROBAR, NO ESTAMOS USANDO MALLOC
				
				/////////////////////////////////////
				/* aca deberia correr instrucciones*/
				/////////////////////////////////////

               //free(proceso);

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
	while(!desconexion_kernel_interrupt){
		int cod_op = recibir_operacion(fd_kernel_interrupt); // recv() es bloqueante por ende no queda loopeando infinitamente
		switch(cod_op){
			//case PROTOCOLOS_A_DEFINIR:
			case HANDSHAKE_KERNEL:
				aceptar_handshake(logger_cpu, fd_kernel_interrupt, cod_op);
				break;
			case INTERRUPCION:
				motivo_interrupcion motivo = recibir_interrupcion();
				enviar_proceso_por_paquete(EXEC, NULL,fd_cpu_interrupt, motivo);
				
			//	break;

			
			//aca deberia ir el case de la interrupcion, recibir el pid, y si esta corrieno interrumpirlo


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
	motivo_interrupcion motivo = extraer_int_del_buffer(buffer);
	destruir_buffer(buffer);
	return motivo;
}