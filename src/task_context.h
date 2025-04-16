class ITaskContext
{
public:
	virtual void OnCompleted() = 0;
	virtual ~ITaskContext() {}
};