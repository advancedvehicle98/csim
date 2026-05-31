TGT = csim

OBJ = main.o \
	cluster/destroy.o \
	cluster/init.o \
	cluster/simulate.o \
	cluster/simulation_step.o \
	cluster/internal/parse_file_wrapper.o \
	cluster/interconnect/node_network_from_dataset.o \
	cluster/job/copy_job.o \
	cluster/job/print_job.o \
	cluster/job/print_job_list.o \
	cluster/job/print_job_truncated.o \
	cluster/job/update_job.o \
	cluster/job_sequencer/add_job_moment.o \
	cluster/job_sequencer/job_sequence_from_dataset.o \
	cluster/node/check_node_for_job.o \
	cluster/node/get_feature_string.o \
	cluster/node/print_node_status.o \
	cluster/node/put_job_to_node.o \
	cluster/node/remove_job_from_node.o \
	cluster/node/update_node.o \
	cluster/statistics/print_statistics.o \
	schedule_algorithms/schedule_fifo.o

BUILD_DIR = ./build
SRC_DIR   = ./src
INC_DIR   = ./include

CFLAGS = \
	-O2 -g -DCONFIG_WITH_DEBUG_PRINT \
	-I $(INC_DIR)
LDFLAGS = -lpthread

OBJ_IN_BUILD_DIR = $(foreach O,$(OBJ),$(BUILD_DIR)/$(O))

COMPONENTS = interconnect internal job job_sequencer node statistics


all: build_dir link


build_dir:
	mkdir -p $(BUILD_DIR) \
		$(BUILD_DIR)/cluster \
		$(BUILD_DIR)/schedule_algorithms \
		$(foreach C,$(COMPONENTS),$(BUILD_DIR)/cluster/$(C))


link: $(OBJ_IN_BUILD_DIR)
	gcc $^ $(LDFLAGS) -o $(BUILD_DIR)/$(TGT)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	gcc $^ -c $(CFLAGS) -o $@


clean:
	rm -rf $(BUILD_DIR) *~ \
		$(SRC_DIR)/*~ \
		$(SRC_DIR)/cluster/*~ \
		$(SRC_DIR)/schedule_algorithms/*~ \
		$(INC_DIR)/*~ \
		$(INC_DIR)/cluster/*~ \
		$(foreach C,$(COMPONENTS),$(SRC_DIR)/cluster/$(C)/*~)


JOB_DS     ?= ./example_job_dataset.txt
CLUSTER_DS ?= ./example_cluster_dataset.txt
SCHED_TYPE ?= fifo

run:
	$(BUILD_DIR)/$(TGT) $(JOB_DS) $(CLUSTER_DS) $(SCHED_TYPE)
