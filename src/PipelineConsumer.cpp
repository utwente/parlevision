import java.util.Map;

public abstract class PipelineConsumer extends PipelineElement {
	
	public PipelineConsumer() {
	}
	
	public abstract void consume( Map<String, Object> in );
}
