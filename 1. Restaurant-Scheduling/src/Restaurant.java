import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;

import static java.lang.Math.max;


public class Restaurant extends Thread {
    public static void main(String[] args) throws IOException {
        File file = new File("src/input.txt");
        FileReader fr=new FileReader(file);   //reads the file
        BufferedReader br=new BufferedReader(fr);  //creates a buffering character input stream
        StringBuffer sb=new StringBuffer();    //constructs a string buffer with no characters
        String line;
        while((line=br.readLine())!=null)
        {
            sb.append(line);      //appends line to string buffer
            sb.append("\n");     //line feed
        }
        fr.close();


        String s = String.valueOf(sb);
        String[] result = s.split("\n");
        int numDiners = Integer.parseInt(result[0]);
        int numTables = Integer.parseInt(result[1]);
        int numCooks = Integer.parseInt(result[2]);
        ArrayList<Integer> arrivalTimes = new ArrayList<>();
        HashMap<Integer, ArrayList<Integer>> dinerOrders = new HashMap<>();

        for (int i=3; i<numDiners + 3; ++i) {
            String l = String.valueOf(result[i]);
            String[] lres = l.split(",");
            arrivalTimes.add(Integer.parseInt(lres[0]));
            ArrayList<Integer> order = new ArrayList<>();
            order.add(Integer.parseInt(lres[1]));
            order.add(Integer.parseInt(lres[2]));
            order.add(Integer.parseInt(lres[3]));
            dinerOrders.put(i-3, order);
        }

        MyObj myObj = new MyObj(numDiners, numTables, numCooks, dinerOrders);

        ArrayList<Thread> dinerThreads = new ArrayList<>();
        for (int i=0; i<numDiners; ++i) {
            Thread t = new Diner(myObj, i, arrivalTimes.get(i));
            t.start();
            dinerThreads.add(t);
        }

        ArrayList<Thread> cookThreads = new ArrayList<>();
        for (int i=0; i<numCooks; ++i) {
            Thread t = new Cook(myObj, i);
            t.start();
            cookThreads.add(t);
        }

        for (Thread t : dinerThreads) {
            try {
                t.join();
            } catch (InterruptedException ex) {
                throw new RuntimeException(ex);
            }
        }

        for (Thread t : cookThreads) {
            try {
                t.join();
            } catch (InterruptedException ex) {
                throw new RuntimeException(ex);
            }
        }
    }
}

class MyObj extends Thread {
    int num_diners, num_waiting_diners, num_cooks, num_tables;
    ArrayList<Integer> allTables = new ArrayList<>();
    ArrayList<Integer> tablesQueue = new ArrayList<>();
    public HashMap<Integer, Integer> tablesTimestamsps = new HashMap<>();
    public HashMap<Integer, ArrayList<Integer>> tableOrderMap = new HashMap<>();
    public HashMap<Integer, ArrayList<Integer>> ordersMap = new HashMap<>();
    public ReentrantLock dinerWaitLock;
    public ReentrantLock dinerLock;
    public Condition dinerCondition;
    public ReentrantLock cookServiceLock;
    public ReentrantLock cookLock;
    public Condition cookCondition;
    public ReentrantLock allTablesLock;
    public ReentrantLock tablesQueueLock;
    public HashMap<Integer, ReentrantLock> tableLockMap = new HashMap<>();
    public HashMap<Integer, Condition> tableConditionMap = new HashMap<>();
    public HashMap<Integer, String> Events = new HashMap<>();
    public HashMap<Integer, Integer> tableDinerMap = new HashMap<>();
    public HashMap<Integer, Integer> tableCookMap = new HashMap<>();

    public MyObj(int num_diners, int num_tables, int num_cooks,
                      HashMap<Integer, ArrayList<Integer>> dinerOrders) {
        this.dinerLock = new ReentrantLock();
        this.dinerCondition = dinerLock.newCondition();

        this.cookLock = new ReentrantLock();
        this.cookCondition = cookLock.newCondition();

        this.dinerWaitLock = new ReentrantLock();
        this.cookServiceLock = new ReentrantLock();

        this.num_diners = num_diners;
        this.num_waiting_diners = 0;
        this.num_cooks = num_cooks;
        this.num_tables = num_tables;
        this.ordersMap = dinerOrders;

        this.allTablesLock = new ReentrantLock();
        this.tablesQueueLock = new ReentrantLock();

        for (int i=0; i<this.num_tables; ++i) {
            this.allTables.add(i);
            ReentrantLock tableLock = new ReentrantLock();
            this.tableLockMap.put(i, tableLock);
            this.tableConditionMap.put(i, tableLock.newCondition());
            tablesTimestamsps.put(i, 0);
        }
    }
}

class Diner extends Thread {
    final MyObj obj;
    int dinerIndex;
    int arrivalTime;
    public Diner(MyObj myObj, int dinerIndex, int arrivalTime) {
        this.obj = myObj;
        this.dinerIndex = dinerIndex;
        this.arrivalTime = arrivalTime;
    }

    synchronized int getEmptyTable() throws InterruptedException {
        this.obj.allTablesLock.lock();
        boolean isEmpty = obj.allTables.isEmpty();
        this.obj.allTablesLock.unlock();
        while (isEmpty) {
//                System.out.println("Diner " + this.dinerIndex + " waiting ...");

            obj.dinerLock.lock();
            try {
                obj.dinerCondition.await();
            } finally {
                obj.dinerLock.unlock();
            }
            this.obj.allTablesLock.lock();
            isEmpty = obj.allTables.isEmpty();
            this.obj.allTablesLock.unlock();
        }

        this.obj.allTablesLock.lock();
        obj.num_diners--;
        int tableIndex = obj.allTables.remove(0);
        this.obj.allTablesLock.unlock();
        obj.tableDinerMap.put(tableIndex, dinerIndex);
        return tableIndex;
    }
    void eatDinner() {
        try {
            int tableIndex = this.getEmptyTable();
            ArrayList<Integer> order;
            ReentrantLock tableLock = obj.tableLockMap.get(tableIndex);
            tableLock.lock();
            try {
                obj.tablesQueue.add(tableIndex);
                int currTime = obj.tablesTimestamsps.get(tableIndex);
                if (currTime < this.arrivalTime) {
                    currTime = this.arrivalTime;
                }
                obj.tablesTimestamsps.put(tableIndex, currTime);
                String s = "[" + currTime + "] Diner " + dinerIndex + " is seated at table " + tableIndex;
                System.out.println(s);
                Thread.sleep(50);
                obj.Events.put(currTime, s);

                order = obj.ordersMap.get(dinerIndex);
                obj.tableOrderMap.put(tableIndex, order);

                Condition tableCondition = obj.tableConditionMap.get(tableIndex);

                obj.cookLock.lock();
                try {
                    obj.cookCondition.signalAll();
                } finally {
                    obj.cookLock.unlock();
                }

                tableCondition.await();
            } finally {
                tableLock.unlock();
            }

            this.obj.allTablesLock.lock();
            obj.allTables.add(tableIndex);
            this.obj.allTablesLock.unlock();

            obj.dinerLock.lock();
            try {
                int currTime = obj.tablesTimestamsps.get(tableIndex);
                currTime = currTime + 30;
                Thread.sleep(150);
                obj.tablesTimestamsps.put(tableIndex, currTime);
                String s = "[" + currTime + "] Diner " + dinerIndex + " leaves.";
                System.out.println(s);
                Thread.sleep(50);
                obj.Events.put(currTime, s);
                obj.dinerCondition.signalAll();
            } finally {
                obj.dinerLock.unlock();
            }
        } catch(InterruptedException exception) {
            Thread.currentThread().interrupt();
        }
    }

    public void run() {
        eatDinner();
    }
}

class Cook extends Thread {
    final MyObj obj;
    int cookIndex;
    int cookTimeSignature;
    public Cook(MyObj myObj, int cookIndex) {
        this.obj = myObj;
        this.cookIndex = cookIndex;
        this.cookTimeSignature = 0;
    }

    synchronized int getWaitingTable() throws InterruptedException {
        this.obj.tablesQueueLock.lock();
        boolean isEmpty = obj.tablesQueue.isEmpty();
        this.obj.tablesQueueLock.unlock();
        while (isEmpty) {
            obj.cookLock.lock();
            try {
                obj.cookCondition.await();
            } finally {
                obj.cookLock.unlock();
            }

            this.obj.tablesQueueLock.lock();
            isEmpty = obj.tablesQueue.isEmpty();
            this.obj.tablesQueueLock.unlock();
        }

        this.obj.tablesQueueLock.lock();
        obj.num_cooks--;
        int tableIndex = obj.tablesQueue.remove(0);
        this.obj.tablesQueueLock.unlock();
        obj.tableCookMap.put(tableIndex, cookIndex);
        return tableIndex;
    }

    private void serveDinner() {
        try {
            int tableIndex = this.getWaitingTable();
            int currTime = max(obj.tablesTimestamsps.get(tableIndex), this.cookTimeSignature);
            obj.tablesTimestamsps.put(tableIndex, currTime);
            int dinerIndex = obj.tableDinerMap.get(tableIndex);
            Thread.sleep(50);
            String s = "[" + currTime + "] Diner " + dinerIndex + "'s order will be processed by " + cookIndex;
            System.out.println(s);
            Thread.sleep(50);
            obj.Events.put(currTime, s);

            ArrayList<Integer> order;
            ReentrantLock tableLock = obj.tableLockMap.get(tableIndex);
            tableLock.lock();
            try {
                order = obj.tableOrderMap.remove(tableIndex);
                serviceOrder(order, tableIndex, cookIndex);
                currTime = obj.tablesTimestamsps.get(tableIndex);
                this.cookTimeSignature = currTime;
                s = "[" + currTime + "] Diner " + dinerIndex + "'s food is ready. Diner " + dinerIndex + " starts to eat.";
                System.out.println(s);
                Thread.sleep(50);
                obj.Events.put(currTime, s);
                Condition tableCondition = obj.tableConditionMap.get(tableIndex);
                tableCondition.signalAll();
            } finally {
                tableLock.unlock();
            }


            obj.num_cooks++;
        } catch(InterruptedException exception) {
            Thread.currentThread().interrupt();
        }
    }

    private synchronized void serveBurger(int tableIndex, int cookIndex) throws InterruptedException {
        int currTime = obj.tablesTimestamsps.get(tableIndex);
        currTime += 5;
        obj.tablesTimestamsps.put(tableIndex, currTime);
        Thread.sleep(50);
        System.out.println("[" + currTime + "] Cook " + cookIndex + " takes the machine for Buckeye Burger.");
    }

    private synchronized void serveFries(int tableIndex, int cookIndex) throws InterruptedException {
        int currTime = obj.tablesTimestamsps.get(tableIndex);
        currTime += 3;
        obj.tablesTimestamsps.put(tableIndex, currTime);
        Thread.sleep(30);
        System.out.println("[" + currTime + "] Cook " + cookIndex + " takes the machine for Brutus Fries.");
    }

    private synchronized void serveSoda(int tableIndex, int cookIndex) throws InterruptedException {
        int currTime = obj.tablesTimestamsps.get(tableIndex);
        currTime += 1;
        obj.tablesTimestamsps.put(tableIndex, currTime);
        Thread.sleep(10);
        System.out.println("[" + currTime + "] Cook " + cookIndex + " takes the machine for Coke.");
    }

    public void serviceOrder(ArrayList<Integer> order, int tableIndex, int cookIndex) throws InterruptedException {
        int numBurgers = order.get(0);
        for (int i = 0; i < numBurgers; ++i) {
            serveBurger(tableIndex, cookIndex);
        }

        int numFries = order.get(1);
        for (int i = 0; i < numFries; ++i) {
            serveFries(tableIndex, cookIndex);
        }

        int numSoda = order.get(2);
        for (int i = 0; i < numSoda; ++i) {
            serveSoda(tableIndex, cookIndex);
        }
    }

    public void run() {
        while (this.obj.num_diners >= -1) {
            serveDinner();
        }
    }
}
