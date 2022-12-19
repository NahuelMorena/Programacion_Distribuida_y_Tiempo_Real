import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import jade.core.*;
public class AgenteMovil extends Agent {
	private List<String> containers = new ArrayList<String>();
	private Integer[] results = new Integer[3]; 
	private int position = 0;
	private String file_name = null;
	//Cantidad de contenedores a utilizar
	private int cant_containers = 3;
	
	private void presentation(Location origen) {
		System.out.println("\n\n--------------------------------------------\nPresentación:");
		System.out.println("Hola, agente con nombre local " + getLocalName());
		System.out.println("Y nombre completo... " + getName());
		System.out.println("Y en location " + origen.getID());
		System.out.println("---------------------------------------------\n\n");
	}
	
	// Ejecutado por nica vez en la creacion
	public void setup() {
		Location origen = here();
		this.presentation(origen);
	
		//Se toma el nombre del archivo pasado como argumento
		Object[] args = getArguments();
		if (args == null) {
			System.out.println("ERROR-- Se requiere nombre de archivo a operar como argumento");
			System.exit(0);
		} else {
			file_name = args[0].toString();
			System.out.println("Se trabaja con el archivo "+ file_name);
		}
		
		//Se genera el arreglo de contenedores
		for (int i = 1; i <= cant_containers; i++) {
			containers.add("Container-"+i);
		}
		containers.add("Main-Container");
		
		// Para migrar el agente	
		try {
			ContainerID destino = new ContainerID(containers.get(position), null);
			position++;
			System.out.println("Migrando el agente a " + destino.getID());
			doMove(destino);
		} catch (Exception e) {
			System.out.println("No fue posible migrar el agente\n\n\n");
		}
	}

	// Ejecutado al llegar a un contenedor como resultado de una migracin
	protected void afterMove() {
		Location origen = here();
		this.presentation(origen);;
		
		if (!origen.getName().equals("Main-Container")) {
			
			//Se suma los numeros de los archivos
			int number;
			results[position - 1] = 0;
			try {
				System.out.println("Se solicita el archivo: "+ origen.getName()+"/"+ file_name);
				FileReader file = new FileReader("myexamples/"+origen.getName()+"/"+ file_name);
				number = file.read();
				number = Character.getNumericValue(number);
				
				while (number != -1) {
					results[position - 1] += number;
					number = file.read();
					number = Character.getNumericValue(number);
				}
				file.close();
			} catch (FileNotFoundException e1) {
				System.out.println("El contenedor " +origen.getID()+" no posee el archivo solicitado");
			} catch (IOException e) {
				System.out.println("Se produjo un error en la lectura del archivo");
			}
			
			System.out.println("Se completo la operación\nResultado obtenido: "+ results[position - 1]);
					
			ContainerID destino;
			try {
				destino = new ContainerID(containers.get(position), null);
				position++;
				System.out.println("Migrando el agente a " + destino.getID());
				doMove(destino);
			} catch (Exception e) {
				System.out.println("No fue posible migrar el agente\n\n\n");
			}
		//Ejecución del Main-Container
		} else {
			int tot = 0;
			for (int i = 0; i < cant_containers; i++) {
				System.out.println("Computadora: "+ containers.get(i) +"\n Resultado de la suma obtenida: "+ results[i]);
				tot += results[i];
			}
			System.out.println("Suma total entre todos los archivos: " + tot);
			System.out.println("Finaliza la operación");
		}
	}
}
