#ifndef _PCNN_H_
#define _PCNN_H_

#include "network.h"

#include <vector>

#define OUTPUT_ACTIVE_STATE			(double) 1.0
#define OUTPUT_INACTIVE_STATE		(double) 0.0

typedef struct pcnn_oscillator {
	double output;
	double feeding;
	double linking;
	double threshold;

	pcnn_oscillator(void) :
		output(0.0),
		feeding(0.0),
		linking(0.0),
		threshold(0.0) { }
} pcnn_oscillator;


typedef struct pcnn_parameters {
	// Multiplier for the feeding compartment at the current step.
    double VF = 1.0;
    
    // Multiplier for the linking compartment at the current step.  
    double VL = 1.0;
    
    // Multiplier for the threshold at the current step.    
    double VT = 10.0;
    
    // Multiplier for the feeding compartment at the previous step.    
    double AF = 0.1;
    
    // Multiplier for the linking compartment at the previous step.
    double AL = 0.1;
    
    // Multiplier for the threshold at the previous step.
    double AT = 0.5;
    
    // Synaptic weight - neighbours influence on linking compartment
    double W = 1.0;
    
    // Synaptic weight - neighbours influence on feeding compartment.
    double M = 1.0;
    
    // Linking strength in the network.
    double B = 0.1;
    
    // Enable/disable Fast-Linking mode. Fast linking helps to overcome some of the effects of time quantisation. This process allows the linking wave to progress a lot faster than the feeding wave.
    bool FAST_LINKING = false;

} pcnn_parameters;

typedef std::vector<unsigned int>		pcnn_ensemble;
typedef std::vector<double>				pcnn_stimulus;
typedef std::vector<unsigned int>		pcnn_time_signal;

typedef struct pcnn_network_state {
public:
	std::vector<double> m_output;

	double				m_time;

public:
	inline size_t size(void) const { return m_output.size(); }

	inline pcnn_network_state & operator=(const pcnn_network_state & other) {
		if (this != &other) {
			m_output.resize(other.size());
			std::copy(other.m_output.cbegin(), other.m_output.cend(), m_output.begin());

			m_time = other.m_time;
		}

		return *this;
	}
} pcnn_network_state;


class pcnn_dynamic : public dynamic_data<pcnn_network_state> {
public:
	pcnn_dynamic(void);

	pcnn_dynamic(const unsigned int number_oscillators, const unsigned int simulation_steps);

	~pcnn_dynamic(void);

public:
	void allocate_sync_ensembles(ensemble_data<pcnn_ensemble> & sync_ensembles) const;

	void allocate_spike_ensembles(ensemble_data<pcnn_ensemble> & spike_ensembles) const;

	void allocate_time_signal(pcnn_time_signal & time_signal) const;
};


class pcnn : public network {
protected:
	std::vector<pcnn_oscillator> m_oscillators;

	pcnn_parameters m_params;

private:
	pcnn(void);

public:
	pcnn(const unsigned int num_osc, const conn_type connection_type, const pcnn_parameters & parameters);

	virtual ~pcnn(void);

public:
	void simulate(const unsigned int steps, const pcnn_stimulus & stimulus, pcnn_dynamic & output_dynamic);


private:
	void calculate_states(const pcnn_stimulus & stimulus);

	void store_dynamic(const unsigned int step, pcnn_dynamic & dynamic);

	void fast_linking(const std::vector<double> & feeding, std::vector<double> & linking, std::vector<double> & output);
};

#endif
