TGT = csim

OBJ = main.o \
	cluster/destroy.o \
	cluster/init.o \
	cluster/simulate.o \
	cluster/simulation_step.o \
	cluster/job/copy_job.o \
	cluster/job_sequencer/add_job_moment.o \
	cluster/job_sequencer/job_sequence_from_dataset.o \
	schedule_algorithms/schedule_fifo.o

BUILD_DIR = ./build
SRC_DIR   = ./src
INC_DIR   = ./include

CFLAGS = \
	-O2 -g \
	-I $(INC_DIR)
LDFLAGS = 

OBJ_IN_BUILD_DIR = $(foreach O,$(OBJ),$(BUILD_DIR)/$(O))


all: build_dir link


build_dir:
	mkdir -p $(BUILD_DIR) \
		$(BUILD_DIR)/cluster \
		$(BUILD_DIR)/cluster/job \
		$(BUILD_DIR)/cluster/job_sequencer \
		$(BUILD_DIR)/schedule_algorithms


link: $(OBJ_IN_BUILD_DIR)
	gcc $^ $(LDFLAGS) -o $(BUILD_DIR)/$(TGT)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	gcc $^ -c $(CFLAGS) -o $@


clean:
	rm -rf $(BUILD_DIR) *~ \
		$(SRC_DIR)/*~ \
		$(SRC_DIR)/cluster/*~ \
		$(SRC_DIR)/cluster/job/*~ \
		$(SRC_DIR)/cluster/job_sequencer/*~ \
		$(SRC_DIR)/scheduler_algorithms/*~ \
		$(INC_DIR)/*~ \
		$(INC_DIR)/cluster/*~


JOB_DS     ?= ./example_job_dataset.txt
CLUSTER_DS ?= ./example_cluster_dataset.txt
SCHED_TYPE ?= fifo

run:
	$(BUILD_DIR)/$(TGT) $(JOB_DS) $(CLUSTER_DS) $(SCHED_TYPE)
