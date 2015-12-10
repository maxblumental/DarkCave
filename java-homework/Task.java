import java.util.concurrent.Callable;

/**
 * Created by Maxim on 12/10/2015.
 */
public class Task<T> {
    private State<T> state;

    public Task(Callable<? extends T> callable) {
        this.state = new New(callable);
    }

    public synchronized T get() {
        if (state.isNew()) {
            T result = state.get();
            state = new Old(result);
        }

        return state.get();
    }

    public interface State<T> {
        T get();

        boolean isNew();
    }

    class New implements State<T> {
        private final Callable<? extends T> callable;

        public New(Callable<? extends T> callable) {
            this.callable = callable;
        }

        @Override
        public T get() {
            System.out.println("new");
            try {
                return callable.call();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public boolean isNew() {
            return true;
        }
    }

    class Old implements State<T> {
        private final T result;

        Old(T result) {
            this.result = result;
        }

        @Override
        public T get() {
            System.out.println("old");
            return result;
        }

        @Override
        public boolean isNew() {
            return false;
        }
    }
}