#!/usr/bin/python3

from udjat import Agent, State

class SampleAgent(Agent):
	def __init__(self,properties):
		super().__init__(properties)

		print('---------------------------------')
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.summary = "A really simple python agent"
		self.label = "Sample"
		self.value = 0

		print('value={} ({})'.format(self.value,self.summary));

		st = self.state	
		print('state: label={} summary={} icon={} level={}'.format(
			st.label,
			st.summary,
			st.icon,
			st.level
		))
	
	def refresh(self,ondemand):
		self.info("-----> Updating agent value")
		self.value = self.value + 1
		print('------------------------------------------------------------')
		#self.state = State(
		#	name= 'sample',
		#	level = 'ready',
		#	label = 'Sample state',
		#	summary= 'Summary for sample state',
		#	body= 'This is the body for the current state, show a more detailed info',
		#	url= 'https://google.com',
		#)
		#self.state = {
		#	'name': 'sample',
		#	'level': 'ready',
		#	'label': 'Sample state',
		#	'summary': 'Summary for sample state',
		#	'body': 'This is the body for the current state, show a more detailed info',
		#	'url': 'https://google.com',
		#}
		print('------------------------------------------------------------')
		print('self.value={}'.format(self.value))
		return False

def AgentFactory(properties):
	return SampleAgent(properties)
