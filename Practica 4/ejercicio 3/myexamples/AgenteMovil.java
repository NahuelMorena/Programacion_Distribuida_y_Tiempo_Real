import jade.core.*;
import java.io.*;
import java.util.HashMap;
import java.util.Random;

public class AgenteMovil extends Agent
{
	//operación y nombre del archivo.
	private String operation = "";
	private String fileName = "";
	//Ubicacion o destino del archivo
	private String fileLocation = null;
	//Para controlar la lectura y escritura del archivo.
	private int position = 0;
	private int read_size = 4096;
	private byte[] data = null;
	private int bytes_read;
	//Verifica si finaliza la operación.
	private boolean finish = false;
	private int cont = 0; //?

	/*Este hashmap tiene para cada archivo su ubicacion en el DFS. Por ej.
		Song.mp3 -> DataServer2
	*/
	private HashMap<String, String> filesOfServer = new HashMap<String, String>();

	private boolean debug = false;

	private boolean readAndCopy = false;

	//Hash functions

	/*Este metodo carga el hashmap filesOfServer con todos
	 * los nombres de los archivos del DFS y su ubicacion.
	 */
	private void loadHashMap(String filePath) {
		//Cargo el hashmap con el contenido del archivo
		try {
			File file = new File(filePath);					//creates a new file instance  
			FileReader fr = new FileReader(file);			//reads the file  
			BufferedReader br = new BufferedReader(fr);		//creates a buffering character input stream  
			String line; String[] tuple;
			while((line = br.readLine())!=null) {
				tuple = line.split("\t");
				this.filesOfServer.put(tuple[0], tuple[1]);
			}
			fr.close();		//closes the stream and release the resources  
		} catch(IOException e) {
			e.printStackTrace();
		}
	}

	/*Si se ha creado un nuevo archivo se lo agrega al archivo del hashmap. */
	private void appendNewFileToHashmapFile(String fileName, String fileLocation) {
		//Agrego al hasmap file una nueva entrada
        try {
        	File file = new File("ServerDFSLocalSpace/hashmap.txt");
        	FileOutputStream outStream = new FileOutputStream(file, file.exists());
			byte[] buffer = (fileName + "\t" + fileLocation).getBytes();
			outStream.write(buffer, 0, buffer.length);
			outStream.close();
        } catch (Exception e) {
        	System.out.println("Error occurred: "+ e.getMessage());
        }
	}

	/*Si se ha creado un nuevo archivo se lo agrega al archivo del hashmap. */
	private void makeACopyOf(String fileName, String fileLocation) {
        int bytes_read = 0;
		int position = 0;
		int read_size = 4096;
		byte[] data = null;
		//Hago una copia en la ubicacion indicada del archivo
        try {
			do {
				InputStream inStream = new FileInputStream(fileLocation + "LocalSpace/" + fileName);
				data = new byte[read_size];
				inStream.skip(position);
				bytes_read = inStream.read(data, 0, read_size);
				inStream.close();
				//--
				File file = new File(fileLocation + "LocalSpace/" + "copy-" + fileName);
				FileOutputStream outStream = new FileOutputStream(file, file.exists());
				if (bytes_read > 0) {
					outStream.write(data, 0, bytes_read);
					data = null;
					outStream.close();
					position = position + bytes_read;
				}
			} while (bytes_read > 0);
			this.readAndCopy = false;
			//Agregar al hashmap y persistirlo
			this.filesOfServer.put("copy-" + fileName, this.fileLocation);
			this.appendNewFileToHashmapFile("copy-" + fileName, this.fileLocation);
        } catch (Exception e) {
        	System.out.println("Error occurred: "+ e.getMessage());
        }

	}

	/*En nuestro caso este método se va a ejecutar solo en el contenedor cliente,
	el cual necesita leer o escribir en el DFS.*/
	public void setup() {

		//Tomo los argumentos
		Object[] args = getArguments();

		Location location = here();

		if (debug) {
			System.out.println("\nSETUP\n");
			System.out.println("\nHola, soy el agente migrado mi nombre local es " + getLocalName());
			System.out.println("Mi nombre completo... " + getName());
			System.out.println("Estoy en la location " + location.getID() + "\n");
		}

		//Estoy en el cliente, determino la operacion a realizar antes de ir al ServerDFS.
		try {
			//Tomo los argumentos, la operacion y el nombre del archivo
			this.operation = args[0].toString();
			this.fileName = args[1].toString();
			if (this.operation.equals("-r")) {
				try {
					//Si el lectura puede tener el argumento de -r con -c
					//Que significa lectura con copia en el server, lo que
					//pide el punto 3b.
					if (args[2].toString().equals("-c")) {
						//La lectura es con copia en el server
						System.out.println("CON COPIA");
						this.readAndCopy = true;
					} else { System.out.println(args[2].toString()); }
				} catch (ArrayIndexOutOfBoundsException e) {
					System.out.println("Read without copy on Server");
				}
			}
		} catch (ArrayIndexOutOfBoundsException e) {
			System.out.println("Error: Missing arguments");
			System.exit(0);
		}

		/*Migro el agente al contenedor ServerDFS, el cual atendera mi solicitud*/
		try {
			ContainerID destino = new ContainerID("ServerDFS", null);
			System.out.println("Migrando el agente a " + destino.getID());
			doMove(destino);
		} catch (Exception e) {
			System.out.println("\nNo fue posible migrar el agente\n");
		}

	}

	/* Este metodo se ejecuta al llegar a un contenedor como resultado de una migracin */
	protected void afterMove() {

		Location location = here();

		if (debug) {
			System.out.println("\nAFTER_MOVE\n");
			System.out.println("\nHola, soy el agente migrado mi nombre local es " + getLocalName());
			System.out.println("Mi nombre completo... " + getName());
			System.out.println("Estoy en la location " + location.getID() + "\n");
		}

		//SI ESTOY EN EL SERVIDOR DE DFS
		if (location.getName().equals("ServerDFS")) {
			if (debug)
				System.out.println("Estoy en el ServerDFS");

			//Cargo en el hasmap la ubicacion de los archivos del filesystem distribuido
			this.loadHashMap(location.getName() + "LocalSpace/hashmap.txt");
			
			if (this.operation.equals("-r")) { //OPERACION DE LECTURA
				//Chequeo si el archivo existe
				if (this.filesOfServer.keySet().contains(fileName)) {
					//Si existe me fijo en que contenedor esta
					this.fileLocation = this.filesOfServer.get(fileName);
					System.out.println("El archivo existe y esta en el contenedor " + this.fileLocation);
					//Ahora me voy al DataServer para empezar a leer
					ContainerID destino = new ContainerID(this.fileLocation, null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				} else {
					System.out.println("The file does not exist");
					ContainerID destino = new ContainerID("Client", null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				}
			} else { //OPERACION DE ESCRITURA
				//Si no es de lectura la operacion es de escritura
				//Verifico que el archivo no exista, en tal caso decido en donde guardarlo
				if (!this.filesOfServer.keySet().contains(fileName)) {
					//Uso la clase random para mandar el archivo a un server de datos al azar
					if ((new Random()).nextInt(2) == 0) {
						this.fileLocation = "DataServer1";
						System.out.println("El archivo ira al DataServer1");
					} else {
						this.fileLocation = "DataServer2";
						System.out.println("El archivo ira al DataServer2");
					}
					//Ahora me voy al Cliente para empezar a leer, luego para
					//escribir este ira al DataServer adecuado.
					ContainerID destino = new ContainerID("Client", null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				} else {
					//Si existe me fijo en cual data server esta
					this.fileLocation = this.filesOfServer.get(fileName);
					//Ahora me voy al Cliente para empezar a leer, luego para
					//escribir este ira al DataServer adecuado.
					ContainerID destino = new ContainerID("Client", null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				}
			}
		} else {
			if (location.getName().equals("DataServer1") || location.getName().equals("DataServer2")) {
				if (this.operation.equals("-r")) {
					//Es una operacion de lectura sobre el server
					//Antes, me fijo si la lectura es con copia, en tal caso primero hago la copia
					System.out.println(this.readAndCopy);
					if (this.readAndCopy) {
						//HACER COPIA DEL ARCHIVO
						//Despues de hacer la copia desactivo el flag para que nunca mas vuelva a copiarlo
						this.makeACopyOf(fileName, fileLocation);
					}
					//leo el archivo
					this.read(location.getName() + "LocalSpace/" + fileName);
					//Vuelvo al cliente para seguir escribiendo
					ContainerID destino = new ContainerID("Client", null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				} else {
					//Es una operacion de escritura sobre el server
					//Escribo la parte del archivo
					this.write(location.getName() + "LocalSpace/" + fileName);
					//Vuelvo al cliente para seguir leyendo
					ContainerID destino = new ContainerID("Client", null);
					System.out.println("Migrando el agente a " + destino.getID());
					doMove(destino);
				}
			} else {
				//Es el contenedor Cliente
				if (this.operation.equals("-r")) {
					//Es una operacion de lectura sobre el server
					if (!(bytes_read == 0)) {
						//Escribo
						this.write(location.getName() + "LocalSpace/" + fileName);
						//Me fijo si la operacion termino....
						if (!finish) {
							//Vuelvo al DataServer a buscar mas
							ContainerID destino = new ContainerID(this.fileLocation, null);
							System.out.println("Migrando el agente a " + destino.getID());
							doMove(destino);
						} else {
							System.out.println("Lectura completada con exito");
						}
					} else {
						System.out.println("El archivo no existe");
					}
				} else {
					//Es una operacion de escritura sobre el server
					if (!this.finish) {
						this.read(location.getName() + "LocalSpace/" + fileName);
						if (!(bytes_read == 0)) { //Si lei algo
							//Vuelvo al DataServer a escribir mas
							ContainerID destino = new ContainerID(this.fileLocation, null);
							System.out.println("Migrando el agente a " + destino.getID());
							doMove(destino);
						} else {
							System.out.println("El archivo no existe o no se pude leer");
						}
					} else {
						//Agrego el nuevo archivo al hashmap y a su archivo para persistirlo.
						this.filesOfServer.put(fileName, this.fileLocation);
						this.appendNewFileToHashmapFile(fileName, this.fileLocation);
						System.out.println("Escritura completada con exito");
					}
				}
			}
		}

	}

	/**
	 * Lee en bytes la cantidad de datos indicada por la variable read_size el archivo 
	 * que se encuentra en origin_path y asigna los datos leidos en la variable data.
	 */
	private void read(String filePath){
        bytes_read = 0;
        try {
            InputStream inStream = new FileInputStream(filePath);
            data = new byte[read_size];
            inStream.skip(position);
            bytes_read = inStream.read(data, 0, read_size);
            inStream.close();
        } catch(IOException e) {
        	System.out.println("Error occurred: " + e.getMessage());
        }
    }

	/**
	 * Escribe en bytes la cantidad de datos indicada por la variable read_size sobre el
	 * archivo que se encuentra en destiny_path los datos indicados en la variable data.
	 */
    private void write(String filePath){
        try {
        	File file = new File(filePath);
        	FileOutputStream outStream = new FileOutputStream(file, file.exists());
            if (bytes_read > 0) {
            	outStream.write(data, 0, bytes_read);
                data = null;
                outStream.close();
                position = position + bytes_read;
            } else {
                finish = true;
            }
        } catch (Exception e) {
        	System.out.println("Error occurred: "+ e.getMessage());
        }
    }

}
