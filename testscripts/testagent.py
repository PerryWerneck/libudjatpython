#!/usr/bin/python3

from udjat import Agent

class SampleAgent(Agent):
	def setup(self,properties):
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.value = 0
		print('state={}'.format(self.state.summary))
		return super().setup(properties)
	
	def refresh(self,ondemand):
		self.info("Updating values")
		print(self.value)
		return False

def AgentFactory(properties):
	return SampleAgent(properties)
