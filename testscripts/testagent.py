#!/usr/bin/python3

from udjat import logger, agent

class SampleAgent(agent):
	def setup(self,properties):
		self.name = "SampleAgent"
		self.info("Setting up from properties")
		self.value = 0
		return super().setup(properties)
	
	def refresh(self,ondemand):
		self.info("Updating values")
		return False;

def agent_factory(properties):
	agent = SampleAgent(properties)
	agent.info("Agent factory method was called")
	return agent
