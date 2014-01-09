Luna

It's a project for click-through-rate(CTR) prediction.
Luna includes two parts: 
	CTR prediction feature engineering platform
	MPI-Based learning system
	
The learning system contains several projects in folder:"src\main\C++", includes:
	inslookup: Read dataset into memory.
	QWLQN : Project from MSRA, it's the Orthant-Wise Limited-memory Quasi-Newton trainer, L1/L2 norm for linear regression and logistic regression problem.
	myLR : Logistic regression with L1/L2 norm based on OWLQN with customized input data structure.
	myPeval: Evaluate the result and calculate the AUC performance.
	mpiLR: mpi-based Logistic Regression with L1/L2 norm and customized input format.
	mpi-peval: distributed AUC calculation based on MPI-cluster.
	featselect: couple group lasso model for CTR prediction and feature selection.
	mpi-featselect: mpi-version couple group lasso model.
	myPeval-featsel: evaluation project for couple group lasso model.
	mpi-peval-featsel: mpi-version evaluation project for couple group lasso model.
	
