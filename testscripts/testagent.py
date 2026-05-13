#!/usr/bin/python3

from udjat import Agent, State

class SampleAgent(Agent):
	def setup(self,properties):
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.value = 0

		st = self.state	
		print('state: label={} summary={} icon={} level={}'.format(
			st.label,
			st.summary,
			st.icon,
			st.level
		))

		return super().setup(properties)
	
	def refresh(self,ondemand):
		self.info("Updating values")
		print('------------------------------------------------------------')
		state = State(
			name= 'sample',
			level = 'info',
			label = 'Sample state',
			summary= 'Summary for sample state',
			body= 'This is the body for the current state, show a more detailed info',
			url= 'https://google.com',
		)
		print('------------------------------------------------------------')
		print(self.value)
		return False

def AgentFactory(properties):
	return SampleAgent(properties)
